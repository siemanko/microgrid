#ifndef DEMAND_RESPONSE_A_BOX_H
#define DEMAND_RESPONSE_A_BOX_H

extern float off_threshold;
extern float red_threshold;
extern float yellow_threshold;

void init_a_box_demand_response();

void a_box_demand_response_step();

#endif