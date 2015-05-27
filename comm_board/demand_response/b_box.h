#ifndef DEMAND_RESPONSE_B_BOX_H
#define DEMAND_RESPONSE_B_BOX_H

#include "demand_response/common.h"

void init_b_box_demand_response();

int b_box_readings_ready();
float b_box_get_power();

DemandResponeState b_box_demand_reponse_current_state();

void b_box_demand_response_step();

#endif