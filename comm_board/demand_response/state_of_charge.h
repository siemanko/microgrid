#ifndef DEMAND_REPONSE_STATE_OF_CHARGE_H
#define DEMAND_REPONSE_STATE_OF_CHARGE_H

extern float state_of_charge_q;
extern float uncertertainty_of_charge;
extern float battery_capacity_q;

void init_state_of_charge();

void state_of_charge_step();

#endif