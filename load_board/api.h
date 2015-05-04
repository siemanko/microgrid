/* 
 * File:   COMM_BOARD_INTERFACE.h
 * Author: Dan
 *
 * Created on December 27, 2014, 4:47 PM
 */

#ifndef COMM_BOARD_INTERFACE_H
#define	COMM_BOARD_INTERFACE_H

#include <stdint.h>

void set_communication_enable(int enabled);

uint8_t load_board_handle_message(uint8_t msg);

#endif	/* COMM_BOARD_INTERFACE_H */

