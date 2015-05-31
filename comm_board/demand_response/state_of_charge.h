#ifndef DEMAND_REPONSE_STATE_OF_CHARGE_H
#define DEMAND_REPONSE_STATE_OF_CHARGE_H

extern float state_of_charge_q;
extern float uncertertainty_of_charge;
extern float battery_capacity_q;

void init_state_of_charge();

// by percentage I mean a value between 0 and 1.
float get_state_of_charge_percentage();

void state_of_charge_step();

#endif