#include "a_box.h"

#include "data_logger/data_logger.h"
#include "communication/other_boards/link_board.h"
#include "drivers/timer.h"
#include "utils/debug.h"
#include "communication/messages.h"
#include "shared/algorithm/circular_buffer.h"

#define NAN 0.0/0.0

DataLoggerSchema a_box_schema;

static const char* name = "A box logs";
static const uint8_t num_column = 5;
static const DataLoggerType column_type[] = {
    DATA_LOGGER_TYPE_UINT32,
    DATA_LOGGER_TYPE_FLOAT,
    DATA_LOGGER_TYPE_FLOAT,
    DATA_LOGGER_TYPE_FLOAT,
    DATA_LOGGER_TYPE_FLOAT,

};
static const char* column_name[] = {
    "timestamp",
    "battery input current",
    "battery output current",
    "network voltage",
    "battery voltage"
};

int validate_data(float battery_input_current,
                  float battery_output_current,
                  float network_voltage,
                  float battery_voltage) {
    int ok = 1;
    ok = ok && -0.1 <= battery_input_current && battery_input_current <= 25.0;
    ok = ok && -0.1 <= battery_output_current && battery_output_current <= 25.0;
    ok = ok && 20.0 <= network_voltage && network_voltage <= 30.0;
    ok = ok && 10.0 <= battery_voltage && battery_voltage <= 18.0;
    return ok;
}

void a_box_pull_data(int* success,
                            float* battery_input_current,
                            float* battery_output_current,
                            float* network_voltage,
                            float* battery_voltage) {
    *success =
            link_board_battery_input_current(battery_input_current) &&
            link_board_battery_output_current(battery_output_current) &&
            link_board_network_voltage(network_voltage) &&
            link_board_battery_voltage(battery_voltage);
    if ((*success) && !validate_data(*battery_input_current,
                                     *battery_output_current,
                                     *network_voltage,
                                     *battery_voltage)) {
        *success = 0;
    }
    if (!(*success)) {
        *battery_input_current = NAN;
        *battery_output_current = NAN;
        *network_voltage = NAN;
        *battery_voltage = NAN;
    }
}

void a_box_print_dl_message_handler(Message* msg) {
    int success;
    float battery_input_current;
    float battery_output_current;
    float network_voltage;
    float battery_voltage;
    
    a_box_pull_data(&success,
                    &battery_input_current,
                    &battery_output_current,
                    &network_voltage,
                    &battery_voltage);
    
    debug(DEBUG_INFO, "A box(%d): %f %f %f %f", 
            success,
            battery_input_current,
            battery_output_current,
            network_voltage,
            battery_voltage);
}

void init_a_box_data_logger() {
    init_data_logger();
    //b_box_schema = (DataLoggerSchema){ name, num_column, column_type, column_name});
    a_box_schema.name = name;
    a_box_schema.num_column = num_column;
    a_box_schema.column_type = column_type;
    a_box_schema.column_name = column_name;
    data_logger_load_schema(&a_box_schema);
    set_message_handler(UMSG_PRINT_DATA_LOGS, a_box_print_dl_message_handler);
}

void a_box_data_logger_step() {
    int success;
    float battery_input_current;
    float battery_output_current;
    float network_voltage;
    float battery_voltage;
    
    a_box_pull_data(&success,
                    &battery_input_current,
                    &battery_output_current,
                    &network_voltage,
                    &battery_voltage);
    
    debug(DEBUG_INFO, "A box(%d): %f %f %f %f", 
            success,
            battery_input_current,
            battery_output_current,
            network_voltage,
            battery_voltage);
    data_logger_log(0,
                    time_seconds_since_epoch(),
                    battery_input_current,
                    battery_output_current,
                    network_voltage,
                    battery_voltage);
}