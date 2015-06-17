#include "load_board.h"

#include "drivers/spi.h"
#include "utils/debug.h"

#define TURN_LIGHT_ON_MSG                           2
#define TURN_LIGHT_OFF_MSG                          3
#define TURN_PORT1_ON_MSG                           4
#define TURN_PORT1_OFF_MSG                          5
#define TURN_PORT2_ON_MSG                           6
#define TURN_PORT2_OFF_MSG                          7

#define GET_CURRENT_FROM_LOAD_BOARD_MSG             8
#define GET_LINE_VOLTAGE_FROM_LOAD_BOARD_MSG        9
#define GET_PORT1_VOLTAGE_FROM_LOAD_BOARD_MSG       10
#define GET_PORT2_VOLTAGE_FROM_LOAD_BOARD_MSG       11

// WARNING: 11 is not allowed. It's used as a part of hacky
// spi protocol

void init_load_board_interface(){
    init_spi();
}

int load_board_output_current(float* result){
    return spi_request_float(GET_CURRENT_FROM_LOAD_BOARD_MSG, result);
}

int load_board_network_voltage(float* result){
    return spi_request_float(GET_LINE_VOLTAGE_FROM_LOAD_BOARD_MSG, result);
}

int load_board_output_voltage(float* result){
    return spi_request_float(GET_PORT1_VOLTAGE_FROM_LOAD_BOARD_MSG, result);
}

int load_board_phone_voltage(float* result){
    return spi_request_float(GET_PORT2_VOLTAGE_FROM_LOAD_BOARD_MSG, result);
}

int load_board_ports_off(){
    int success = spi_command(TURN_PORT1_OFF_MSG) &&
                  spi_command(TURN_PORT2_OFF_MSG) &&
                  spi_command(TURN_LIGHT_OFF_MSG);
    //debug(DEBUG_INFO, "sent OFF signal to all appliances: %d", success);
    return success;
}

int load_board_ports_on(){
    int success = spi_command(TURN_PORT1_ON_MSG) &&
                  spi_command(TURN_PORT2_ON_MSG) &&
                  spi_command(TURN_LIGHT_ON_MSG);
    //debug(DEBUG_INFO, "sent ON signal to all appliances: %d", success);
    return success;
}