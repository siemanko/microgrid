/* 
 * File:   LoadBoardCommInterface.h
 * Author: Dan
 *
 * Created on December 20, 2014, 4:54 PM
 */

#ifndef COMMUNICATION_OTHER_BOARDS_LOAD_BOARD_H
#define	COMMUNICATION_OTHER_BOARDS_LOAD_BOARD_H


void init_load_board_interface();

int load_board_output_current(float* result);
int load_board_network_voltage(float* result);
int load_board_output_voltage(float* result);
int load_board_phone_voltage(float* result);

int load_board_ports_on();
int load_board_ports_off();


#endif	/* LOADBOARDCOMMINTERFACE_H */

