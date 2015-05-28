#include "b_box.h"

#include "data_logger/data_logger.h"
#include "communication/other_boards/load_board.h"
#include "drivers/timer.h"
#include "utils/debug.h"
#include "communication/messages.h"

#define NAN 0.0/0.0


DataLoggerSchema b_box_schema;

static const char* name = "B box logs";
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
    "output current",
    "network voltage",
    "output voltage",
    "phone_voltage"
};

void pull_readings(int* success,
                   float* output_current,
                   float* network_voltage,
                   float* output_voltage,
                   float* phone_voltage) {
    *success =
            load_board_output_current(output_current) &&
            load_board_network_voltage(network_voltage) &&
            load_board_output_voltage(output_voltage) &&
            load_board_phone_voltage(phone_voltage);

    if (!(*success)) {
        *output_current = NAN;
        *network_voltage = NAN;
        *output_voltage = NAN;
        *phone_voltage = NAN;
    }
}

void b_box_print_dl_message_handler(Message* msg) {
    int success;
    float output_current;
    float network_voltage;
    float output_voltage;
    float phone_voltage;

    pull_readings(&success,
                  &output_current,
                  &network_voltage,
                  &output_voltage,
                  &phone_voltage);
    
    
    debug(DEBUG_INFO, "B box(%d): %f %f %f %f", 
        success,
        output_current,
        network_voltage,
        output_voltage,
        phone_voltage);
}

void init_b_box_data_logger() {
    init_data_logger();
    //b_box_schema = (DataLoggerSchema){ name, num_column, column_type, column_name});
    b_box_schema.name = name;
    b_box_schema.num_column = num_column;
    b_box_schema.column_type = column_type;
    b_box_schema.column_name = column_name;
    data_logger_load_schema(&b_box_schema);
    set_message_handler(UMSG_PRINT_DATA_LOGS, b_box_print_dl_message_handler);
}




void b_box_data_logger_step() {
    int success;
    float output_current;
    float network_voltage;
    float output_voltage;
    float phone_voltage;

    pull_readings(&success,
                  &output_current,
                  &network_voltage,
                  &output_voltage,
                  &phone_voltage);
    
    
    debug(DEBUG_INFO, "B box(%d): %f %f %f %f", 
        success,
        output_current,
        network_voltage,
        output_voltage,
        phone_voltage);
    
    data_logger_log(0,
                    time_seconds_since_epoch(),
                    output_current,
                    network_voltage,
                    output_voltage,
                    phone_voltage);
}