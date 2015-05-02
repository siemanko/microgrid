/*
 * File:   SPI_INTERFACE.h
 * Author: Dan
 *
 * Created on October 5, 2014, 10:59 PM
 */

#ifndef DRIVERS_EEPROM_H
#define	DRIVERS_EEPROM_H

#include <stdint.h>

void init_eeprom();

// Those functions read from memory
unsigned char eeprom_read_byte(unsigned short address);
float eeprom_read_float(unsigned short address);
int eeprom_read_int(unsigned short address);
uint32_t eeprom_read_uint32(unsigned short address);

// Those functions write to memory
int eeprom_write_byte(unsigned short address, unsigned char data);
void eeprom_write_int(unsigned short address, int data);
void eeprom_write_uint32(unsigned short address, long data);
int eeprom_write_float(unsigned short address, float data);

// Resets all the values to 0
int eeprom_wipe();

#endif

