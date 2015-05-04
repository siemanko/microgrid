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

void init_spi_slave();

void spi_slave_put_byte(uint8_t );

void spi_slave_set_handle_message(uint8_t (*handler)(uint8_t));

#endif	/* SPI_COMM_BOARD_H */

