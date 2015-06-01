#ifndef DEMAND_REPONSE_SOC_INTERPOLATION_H
#define DEMAND_REPONSE_SOC_INTERPOLATION_H

float get_soc_estimate(float voltage,
                       float current_in,
                       float current_out,
                       float battery_capacity_q);

#endif