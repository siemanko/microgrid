#include "link_board.h"

#include "drivers/spi.h"

#define LINK_BATTERY_INPUT_CURRENT                       2
#define LINK_BATTERY_OUTPUT_CURRENT                      3
#define LINK_NETWORK_VOLTAGE                             4
#define LINK_BATTERY_VOLTAGE                             5


void init_link_board_interface() {
    init_spi();
}

int link_board_battery_input_current(float* result){
    return spi_request_float(LINK_BATTERY_INPUT_CURRENT, result);
}

int link_board_battery_output_current(float* result){
    return spi_request_float(LINK_BATTERY_OUTPUT_CURRENT, result);
}

int link_board_network_voltage(float* result){
    return spi_request_float(LINK_NETWORK_VOLTAGE, result);
}

int link_board_battery_voltage(float* result){
    return spi_request_float(LINK_BATTERY_VOLTAGE, result);
}

