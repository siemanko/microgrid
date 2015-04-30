/* 
 * File:   SPI_LOAD_BOARD.h
 * Author: Dan
 *
 * Created on December 17, 2014, 12:31 AM
 */

#ifndef DRIVERS_SPI_H
#define	DRIVERS_SPI_H

#include <stdint.h>

void init_spi();

// sends a command to though SPI.
// expect zero reply on successful execution of
// command. Returns 1 if command was sent and
// executed successfully.
int spi_command(uint8_t command);

// Sends a command and expects byte reply.
int spi_request_byte(uint8_t command, uint8_t* result);

// Sends a command and expect float reply.
int spi_request_float(uint8_t command, float* result);

#endif

