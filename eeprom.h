/*
 * File:   eeprom.h
 * Author: Jayesh Jagtap
 */


#ifndef EEPROM_H
#define EEPROM_H

void eeprom_write(unsigned char addr, unsigned char data);
unsigned char eeprom_read(unsigned char addr);

#endif