#include "reading_model.h"

#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include "isr.h"

double battery_output_current;
double battery_input_current;
double battery_voltage;
double network_voltage;


//////////////////////////////////////////

void update_readings() {
    if (readings_updated) {
        // based on multiplier 3.3/1024.0*5.28
        double battery_voltage = (float)battery_voltage_adc*0.017015625;
        on_battery_voltage_reading(battery_voltage);
        // based on multiplier 3.3/1024.0*11.0
        double network_voltage = (float)network_voltage_adc*0.03544921875;
        on_network_voltage_reading(network_voltage);

        double battery_input_current = ((float)battery_input_current_adc - 512.0) / -28000.0;
        on_battery_input_current_reading(battery_input_current);

        double battery_output_current = ((float)battery_output_current_adc - 512.0) / -28000.0;
        on_battery_output_current_reading(battery_output_current);

        readings_updated = 0;
    }
}

void on_network_voltage_reading(double reading){
    network_voltage = ADC_FORGET*(reading) + (1-ADC_FORGET)*network_voltage;
}

void on_battery_voltage_reading(double reading){
    battery_voltage = ADC_FORGET*(reading) + (1-ADC_FORGET)*battery_voltage;
}

void on_battery_input_current_reading(double reading){
    battery_input_current = ADC_FORGET*(reading) + (1-ADC_FORGET)*battery_input_current;
}
void on_battery_output_current_reading(double reading){
    battery_output_current = ADC_FORGET*(reading) + (1-ADC_FORGET)*battery_output_current;
}

////////////////////////////////////////////////

double get_network_voltage() {
    return network_voltage;
}

double get_battery_voltage() {
    return battery_voltage;
}

double get_battery_input_current() {
    return battery_input_current;
}

double get_battery_output_current() {
    return battery_output_current;
}