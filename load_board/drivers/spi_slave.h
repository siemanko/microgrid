/* 
 * File:   SPI_COMM_BOARD.h
 * Author: Dan
 *
 * Created on December 5, 2014, 8:18 PM
 */

#ifndef SPI_COMM_BOARD_DRIVER_H
#define	SPI_COMM_BOARD_DRIVER_H

#include <stdint.h>

#define LOAD_REQUEST_DATA                           0
#define LOAD_READ_AGAIN                             1

void initSPICommBoard();
void putByte(uint8_t );

#endif	/* SPI_COMM_BOARD_H */

