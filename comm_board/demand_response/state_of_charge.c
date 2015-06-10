#include "state_of_charge.h"

#include "constants.h"
#include "communication/messages.h"
#include "shared/utils.h"
#include "storage.h"
#include "data_logger/a_box.h"
#include "demand_response/soc_interpolation.h"
#include "utils/debug.h"


// battery_capacity
float state_of_charge_q         = 1.0;
// this is sigma^2
float uncertertainty_of_charge  = DEFAULT_BATTERY_CAPACITY_Q;
float battery_capacity_q        = DEFAULT_BATTERY_CAPACITY_Q;

static void set_state_of_charge_handler(Message* msg) {
    assert(msg->length == 9);
    state_of_charge_q = bytes_to_float(msg->content + 1);
    uncertertainty_of_charge = bytes_to_float(msg->content + 5);
}

static void set_battery_capacity_handler(Message* msg) {
    assert(msg->length == 5);
    battery_capacity_q = bytes_to_float(msg->content + 1);
    eeprom_write_float(STORAGE_BATTERY_CAPACITY, battery_capacity_q);
}


void init_state_of_charge() {
    set_message_handler(UMSG_SET_STATE_OF_CHARGE,
            set_state_of_charge_handler);
    set_message_handler(UMSG_SET_BATTERY_CAPACITY,
            set_battery_capacity_handler);

}

float get_state_of_charge_percentage() {
    // TODO: consider using uncertainty as well.
    //return state_of_charge_q / battery_capacity_q;  //DS:  Edit, this should not be commented!
    return 0.95;    
}

//DS:  Edit, uncertain about this because it looks like you are adding Gaussians and it came from Udacity site.
void multiply_gaussians(float mu1, float sigma_sq1,
                        float mu2, float sigma_sq2,
                        float* new_mu, float* new_sigma_sq) {
    double part1 = (double)sigma_sq2*mu1;
    double part2 = (double)sigma_sq1*mu2;
    *new_mu = (part1+part2) / ((double)sigma_sq1 + (double)sigma_sq2);
    double spart1 = (double)1.0/sigma_sq1;
    double spart2 = (double)1.0/sigma_sq2;
    *new_sigma_sq = (double)1.0 / (spart1 + spart2);
}

void state_of_charge_step() {
    int success;
    float battery_input_current;
    float battery_output_current;
    float ignored;
    float battery_voltage;
    
    a_box_pull_data(&success,
                    &battery_input_current,
                    &battery_output_current,
                    &ignored,
                    &battery_voltage);
    if (!success) return;
    
    // motion step
    float motion_delta_mu = battery_input_current - battery_output_current;
    float motion_uncertainty = 
            CURRENT_SENSOR_UNCERTAINTY * (battery_input_current
                                       + battery_output_current);
    state_of_charge_q += motion_delta_mu;
    uncertertainty_of_charge += motion_uncertainty;
            
    
    // measurement step
    float measurement_mu = 
            get_soc_estimate(battery_voltage,
                             battery_input_current,
                             battery_output_current,
                             battery_capacity_q);
    debug(DEBUG_INFO, "battery voltage = %f, current delta = %f",
            battery_voltage, battery_input_current - battery_output_current);
    debug(DEBUG_INFO, "SOC estimate from voltage: %f", measurement_mu);
    
    float measurement_uncertainty =
            VOLTAGE_SENSOR_UNCERTAINTY * measurement_mu;
    
    // It's actually important to use temporaries here
    // not to override results.
    float new_mu, new_uncert;
    multiply_gaussians(state_of_charge_q, uncertertainty_of_charge,
                       measurement_mu, measurement_uncertainty,
                       &new_mu, &new_uncert);
    state_of_charge_q = new_mu;
    uncertertainty_of_charge = new_uncert;
}