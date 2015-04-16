/* 
 * File:   SPI_LOAD_BOARD.h
 * Author: Dan
 *
 * Created on December 17, 2014, 12:31 AM
 */

#ifndef SPI_LOAD_BOARD_H
#define	SPI_LOAD_BOARD_H

#include <stdint.h>

#define SPI_REQUEST_DATA                           0
#define SPI_READ_AGAIN                             11

void initSPI_LoadBoard();

int getFloatLoadBoard(float* result);
int sendLoadBoardByte(uint8_t message, uint8_t* result);
int sendCommandToLoadBoard(int command);
int getFloatForCommand(uint8_t command, float* result);

#endif	/* SPI_LOAD_BOARD_H */

