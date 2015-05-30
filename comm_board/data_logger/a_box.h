#ifndef DATA_LOGGER_A_BOX_H
#define DATA_LOGGER_A_BOX_H

void init_a_box_data_logger();

void a_box_pull_data(int* success,
                     float* battery_input_current,
                     float* battery_output_current,
                     float* network_voltage,
                     float* battery_voltage);

void a_box_data_logger_step();

#endif
