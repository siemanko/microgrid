#ifndef DEMAND_RESPONSE_B_BOX_H
#define DEMAND_RESPONSE_B_BOX_H

#include "demand_response/common.h"

void init_b_box_demand_response();

// when price goes up button needs to be pressed.
// this function is meant to extract that piece of state
// (true if waiting for button).
int waiting_for_confirmation();

int b_box_readings_ready();
float b_box_get_power();

DemandResponeState b_box_demand_reponse_current_state();

void b_box_demand_response_step();

#endif