#include "reading_model.h"

#include <math.h>
#include <stdlib.h>

#include "utils.h"
#include "enable_model.h"


double output_current;
double base_line_output_current = 0.04;

double network_voltage;
double output_voltage;
double phone_voltage;



void on_output_current_reading(double reading) {
    output_current = (ADC_FORGET * reading +
                     (1.0 - ADC_FORGET) * output_current) ;

    // only update it when no appliances are one.
   if (outputs_disabled()) {
        base_line_output_current = ADC_FORGET * reading +
                                  (1.0 - ADC_FORGET) * base_line_output_current;
    }

}

void on_network_voltage_reading(double reading) {
    network_voltage = ADC_FORGET * reading +
                     (1.0 - ADC_FORGET) * network_voltage;
}

void on_output_voltage_reading(double reading) {
    output_voltage = ADC_FORGET * reading +
                     (1.0 - ADC_FORGET) * output_voltage;
}

void on_phone_voltage_reading(double reading) {
    phone_voltage = ADC_FORGET * reading +
                     (1.0 - ADC_FORGET) * phone_voltage;
}


double get_output_current() {
    return  double_max(0.0, output_current - base_line_output_current);
}

double get_network_voltage() {
    return network_voltage;
}

double get_output_voltage() {
    return output_voltage;
}

double get_phone_voltage() {
    return phone_voltage;
}