/* 
 * File:   SPI_POWER_BOARD.h
 * Author: Dan
 *
 * Created on December 5, 2014, 3:09 PM
 */

#include "stdint.h"

#ifndef SPI_POWER_BOARD_H
#define	SPI_POWER_BOARD_H

void initSPI_PowerBoard();
void sendFloatPowerBoard(float data);
void sendIntPowerBoard(int i);
int pollPowerBoard(int input);
float getFloatPowerBoard();

int writeSPI1(int i);
void __attribute__((__interrupt__,auto_psv)) _SPI1Interrupt(void);


#endif	/* SPI_POWER_BOARD_H */

