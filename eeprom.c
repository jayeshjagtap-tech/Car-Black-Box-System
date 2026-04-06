/*
 * File:   eeprom.c
 * Author: Jayesh Jagtap
 */



#include <xc.h>

void eeprom_write(unsigned char addr, unsigned char data)
{
    EEADR = addr;
    EEDATA = data;

    EEPGD = 0;   // Access EEPROM
    WREN = 1;

    GIE = 0;     // Disable interrupts
    EECON2 = 0x55;
    EECON2 = 0xAA;
    WR = 1;
    GIE = 1;     // Enable interrupts

    while(WR);   // Wait till write completes

    WREN = 0;
}

unsigned char eeprom_read(unsigned char addr)
{
    EEADR = addr;

    EEPGD = 0;   // Access EEPROM
    RD = 1;

    return EEDATA;
}