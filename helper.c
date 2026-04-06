/*
 * File:   Helper.c
 * Author: Jayesh Jagtap
 */


#include "main.h"
unsigned char clock_reg[3];
char time[7];
char log[11];
char pos=-1;
int event_count=-1;
extern unsigned char sec,return_time;
char *menu[]={"View log","Clear log","Download log","Set time","Change password"};
void get_time(void){
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD
    
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';
    
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';
    
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
    time[6] = '\0';
}
void display_time(void){
    
    get_time();
    
    clcd_putch(time[0],LINE2(2));
    clcd_putch(time[1],LINE2(3));
    clcd_putch(':',LINE2(4));
    clcd_putch(time[2],LINE2(5));
    clcd_putch(time[3],LINE2(6));
    clcd_putch(':',LINE2(7));
    clcd_putch(time[4],LINE2(8));
    clcd_putch(time[5],LINE2(9));
    
}
void display_dash_board(char event[],unsigned char speed){
    clcd_print("  TIME     E  SP",LINE1(0));
    //display time
    display_time();
    //event
    clcd_print(event, LINE2(11));
    //speed
    clcd_putch((speed/10)+'0',LINE2(14));
    clcd_putch((speed%10)+'0',LINE2(15));
    
    
}
void log_event(){
    unsigned char addr;
    pos++;
    if (pos==10)
    {
        pos=0;//only 10 events displayed in the log
    }
    addr=pos*10+5;
    for(int i=0;log[i]!='\0';i++)
    {
        eeprom_write(addr,log[i]);
        addr++;
    }
    if (event_count<9)
    {
        event_count++;
        
    }
}
void log_car_event(char *event,unsigned char speed)
{
    get_time(); // HHMMSS

    strncpy(log,time,6);        // time
    strncpy(&log[6],event,2);   // event (2 chars)

    log[8]=speed/10+'0';
    log[9]=speed%10+'0';
    log[10]='\0';

    log_event();
}
void clear_screen(void){
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(100);
}

char login(unsigned char key, unsigned char reset_flag){
    //
    static char npassword[4];
    static char spassword[4];
    static unsigned char attempt_rem;
    static unsigned char i;
    if(reset_flag==RESET_PASSWORD)
    {
        return_time=5;
        attempt_rem='3';
        i=0;
        npassword[0]='\0';
        npassword[1]='\0';
        npassword[2]='\0';
        npassword[3]='\0';
        key=ALL_RELEASED;
        
    }
    if(return_time==0){
        return RETURN_BACK;
    }
    if(key == SW4 && i < 4)//sw4 4
    {
        npassword[i]='4';
        clcd_putch('*',LINE2(6+i));
        i++;
        return_time=5;
    }
    else if(key == SW5 && i < 4)//sw5 2
    {
        npassword[i]='2';
        clcd_putch('*',LINE2(6+i));
        i++;
        return_time=5;
        
    }
    if (i==4){
        //compare password to stored password
        for(int j=0;j<4;j++)
        {
            spassword[j]=eeprom_read(j);
        }
        if(strncmp(spassword,npassword,4)==0)
        {
            //menu access
            return TASK_SUCCESS;
            
            
        }
        else
        {
            attempt_rem--;
            if(attempt_rem=='0'){
                //block the user
                clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                __delay_us(100);
                clcd_print("You are Blocked",LINE1(0));
                clcd_print("Wait..for 60sec",LINE2(0));
                sec=60;
                while(sec!=0){
                    clcd_putch(sec/10+'0',LINE2(10));
                    clcd_putch(sec%10+'0',LINE2(11));
                    
                }
                attempt_rem='3';
            }
            else{
                clear_screen();
                clcd_print("Wrong Password",LINE1(1));
                clcd_print("Attempts left",LINE2(0));
                clcd_putch(attempt_rem,LINE2(15));
                __delay_ms(3000);
                
            }
            clear_screen(); 
            clcd_print(" ENTER PASSWORD",LINE1(0));
            clcd_write(CURSOR_POS,INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
            __delay_us(100);
            i=0;
            return_time=5;
            
            
        }
    }
    return 0x10;
    
}
//View logs,clear log,reset password
char login_menu(unsigned char key,unsigned char reset_flag)
{
    
    static char menu_pos;
    static unsigned char once = 1;
    if(reset_flag==RESET_LOGIN_MENU){
        menu_pos=0;
        once=1;
    }
    if(key==SW5 && menu_pos<4){
        menu_pos++;
        clear_screen();
        once=1;
    }
    else if(key==SW4 && menu_pos>0)
    {
       menu_pos--; 
       clear_screen();
       once=1;
    }
    if(once)
    {
        once=0;
        if(menu_pos < 4)
        {
            clcd_putch('*', LINE1(0));
            clcd_print(menu[menu_pos], LINE1(2));
            clcd_print(menu[menu_pos + 1], LINE2(2));
        }
        else
        {
            clcd_print(menu[menu_pos - 1], LINE1(2));
            clcd_print(menu[menu_pos], LINE2(2));
            clcd_putch('*', LINE2(0));
        }
    }
    return menu_pos;
}
void view_log(unsigned char key, unsigned char reset_flag){
    char rlog[11];
    unsigned char addr;
    static unsigned char rpos;
    static unsigned char prev_key = ALL_RELEASED;

    if(event_count == -1)
    {
        clcd_print("NO Logs",LINE2(0));
    }
    else
    {
        if(reset_flag == VIEW_LOG_RESET)
        {
            rpos = 0;
            clear_screen();   
        }
        if(rpos > event_count - 1)
        {
            rpos = 0;
        }

        // ? FIXED condition
        if(key == SW4 && prev_key == ALL_RELEASED && rpos < event_count - 1)// SW4 -> increment (next log)
        {
            rpos++;
            clear_screen();   
        }
        else if(key == SW5 && prev_key == ALL_RELEASED && rpos > 0)// SW5 -> decrement (previous log))
        {
            rpos--;
            clear_screen();  
        }
        prev_key = key;
        addr = rpos * 10 + 5;

        for(int i = 0; i < 10; i++)
        {
            rlog[i] = eeprom_read(addr + i);
        }

        // ? print header (important)
        clcd_print("# TIME     E  SP",LINE1(0));

        clcd_putch(rpos%10 + '0',LINE2(0));

        clcd_putch(rlog[0],LINE2(2));
        clcd_putch(rlog[1],LINE2(3));
        clcd_putch(':',LINE2(4));

        clcd_putch(rlog[2],LINE2(5));
        clcd_putch(rlog[3],LINE2(6));
        clcd_putch(':',LINE2(7));

        clcd_putch(rlog[4],LINE2(8));
        clcd_putch(rlog[5],LINE2(9));

        clcd_putch(rlog[6],LINE2(11));
        clcd_putch(rlog[7],LINE2(12));

        clcd_putch(rlog[8],LINE2(14));
        clcd_putch(rlog[9],LINE2(15)); 
    }
}

char clear_log(unsigned char reset_flag)
{
    
    
    if(reset_flag==RESET_MEMORY){
        event_count=-1;
        pos=-1;
        clcd_print("Logs cleared",LINE1(0));
        clcd_print("Successfully",LINE2(0));
        __delay_ms(3000);
        
        return TASK_SUCCESS;
        
    }
}
char change_password(unsigned char key, unsigned char reset_flag)
{
    static char pwd[9];
    static int pos,once;
    if(reset_flag==RESET_CHANGE_PASSWORD)
    {
      pos=0; 
      once=1;
    }
    if(pos<4 && once){
        once=0;
        clcd_print("Enter new pwd",LINE1(0));
        clcd_write(LINE2(0),INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        
    }
    else if(pos>=4 && once==0){
        once=1;
        clear_screen();
        clcd_print("Re-enter pwd",LINE1(0));
        clcd_write(LINE2(0),INST_MODE);
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
    }
    
    
    if (key==SW4)//4
    {
        pwd[pos]='4';
        clcd_putch('*',LINE2(pos%4));
        pos++;
    }
    else if(key==SW5)//2
    {
        pwd[pos]='2';
        clcd_putch('*',LINE2(pos%4));
        pos++;
    }
    if(pos==8)
    {
        if(strncmp(pwd,&pwd[4],4)==0)
        {
            //passwords verified
            for(int i=0;i<4;i++){
                eeprom_write(i,pwd[i]);
                
            }
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_print("Password changed",LINE1(0));
            clcd_print("Successfully",LINE2(0));
            __delay_ms(3000);
            return TASK_SUCCESS;
            
            
        }
        else//not matching
        {
            clear_screen();
            clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
            __delay_us(100);
            clcd_print("Password change",LINE1(0));
            clcd_print("FAILED",LINE2(0));
            __delay_ms(3000);
            
            return TASK_SUCCESS;
        }
        
        
    }
    return 0x10;
}
void download_log(void)
{
    char rlog[11];
    unsigned char addr;

    for(int i=0;i<=event_count;i++)
    {
        addr = i*10 + 5;

        for(int j=0;j<10;j++)
        {
            rlog[j] = eeprom_read(addr + j);
        }

        // send to serial (UART)
        for(int j=0;j<10;j++)
        {
            while(!TXIF);
            TXREG = rlog[j];
        }

        while(!TXIF);
        TXREG = '\n';
    }
}
char set_time(unsigned char key, unsigned char reset_flag)
{
    static unsigned char pos=0;
    static char new_time[6]={'0','0','0','0','0','0'};

    if(reset_flag==RESET_SET_TIME)
    {
        pos=0;
        clear_screen();   
    }

    clcd_print("Time (HH:MM:SS)",LINE1(0));

    // ? UPDATED DISPLAY FORMAT (with :)
    clcd_putch(new_time[0], LINE2(0));
    clcd_putch(new_time[1], LINE2(1));
    clcd_putch(':',        LINE2(2));

    clcd_putch(new_time[2], LINE2(3));
    clcd_putch(new_time[3], LINE2(4));
    clcd_putch(':',        LINE2(5));

    clcd_putch(new_time[4], LINE2(6));
    clcd_putch(new_time[5], LINE2(7));

    if(key==SW4) // increase
    {
        new_time[pos]++;
        if(new_time[pos]>'9')
            new_time[pos]='0';
    }
    else if(key==SW5) // next digit
    {
        pos++;
        if(pos == 6)
        {
            // write to RTC
            write_ds1307(HOUR_ADDR, ((new_time[0]-'0')<<4)|(new_time[1]-'0'));
            write_ds1307(MIN_ADDR,  ((new_time[2]-'0')<<4)|(new_time[3]-'0'));
            write_ds1307(SEC_ADDR,  ((new_time[4]-'0')<<4)|(new_time[5]-'0'));

            clear_screen();
            clcd_print("TIME UPDATED",LINE1(0));
            __delay_ms(2000);

            return TASK_SUCCESS;
        }
    }
    return 0;
}