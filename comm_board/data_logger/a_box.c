#include "a_box.h"

#include "data_logger/data_logger.h"
#include "communication/other_boards/link_board.h"
#include "drivers/timer.h"
#include "utils/debug.h"

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

void init_a_box_data_logger() {
    init_data_logger();
    //b_box_schema = (DataLoggerSchema){ name, num_column, column_type, column_name});
    a_box_schema.name = name;
    a_box_schema.num_column = num_column;
    a_box_schema.column_type = column_type;
    a_box_schema.column_name = column_name;
    data_logger_load_schema(&a_box_schema);
}

void a_box_data_logger_step() {
    float battery_input_current;
    float battery_output_current;
    float network_voltage;
    float battery_voltage;

    int success =
            link_board_battery_input_current(&battery_input_current) &&
            link_board_battery_output_current(&battery_output_current) &&
            link_board_network_voltage(&network_voltage) &&
            link_board_battery_voltage(&battery_voltage);

    if (!success) {
        battery_input_current = NAN;
        battery_output_current = NAN;
        network_voltage = NAN;
        battery_voltage = NAN;
    }
    
    debug(DEBUG_INFO, "A box: %f %f %f %f", 
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