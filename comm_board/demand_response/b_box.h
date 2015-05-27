#ifndef DEMAND_RESPONSE_B_BOX_H
#define DEMAND_RESPONSE_B_BOX_H

void init_b_box_demand_response();

const char* dr_state_as_string();

int b_box_readings_ready();
float b_box_get_power();

void b_box_demand_response_step();

#endif