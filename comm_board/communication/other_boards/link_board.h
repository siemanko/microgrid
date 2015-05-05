#ifndef COMMUNICATION_OTHER_BOARDS_LINK_BOARD_H
#define	COMMUNICATION_OTHER_BOARDS_LINK_BOARD_H

void init_link_board_interface();

int link_board_battery_input_current(float* result);
int link_board_battery_output_current(float* result);
int link_board_network_voltage(float* result);
int link_board_battery_voltage(float* result);

#endif	/* LINKBOARDINTERFACE_H */