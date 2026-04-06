/* 
 * File:   helper.h
 * Author: Jayesh Jagtap
 */

#ifndef CAR_BLACK_BOX_DEF_H
#define	CAR_BLACK_BOX_DEF_H

void display_dash_board(char event[],unsigned char speed);
void log_car_event(char *event,unsigned char speed);
void clcd_write(unsigned char byte, unsigned char mode);
void clear_screen(void);
char login(unsigned char key, unsigned char reset_flag);
char login_menu(unsigned char key,unsigned char reset_flag);
void view_log(unsigned char key, unsigned char reset_flag);
char clear_log(unsigned char reset_flag);
char change_password(unsigned char key, unsigned char reset_flag);
void download_log(void);
char set_time(unsigned char key, unsigned char reset_flag);

#endif	/* CAR_BLACK_BOX_DEF_H */

