#include "stdint.h"

/* 
 * File:   LibraryFunctions.h
 * Author: Dan
 *
 * Created on January 15, 2015, 10:14 AM
 */


#ifndef LIBRARYFUNCTIONS_H
#define	LIBRARYFUNCTIONS_H

float BytesToFloat(uint8_t raw[]);

void IntToBytes(int source, uint8_t* destination);

int BytesToInt(uint8_t* source);

void LongToBytes(uint32_t source, uint8_t* destination);

uint32_t BytesToLong(uint8_t* source);

void blink_led(void);

void slow_blink(void);

void initRandomNumberGen();

uint16_t getRandomInt();

#endif	/* LIBRARYFUNCTIONS_H */

