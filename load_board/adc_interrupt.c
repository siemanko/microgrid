#include <p33FJ64GS406.h>
#include <stdint.h>

#include "reading_model.h"

#define refv 2.1f

void __attribute__((__interrupt__, no_auto_psv)) _ADCP1Interrupt()
{
    uint16_t current_adc = ADCBUF3;
    uint16_t network_adc = ADCBUF0;
    uint16_t output_adc = ADCBUF1;
    uint16_t phone_adc = ADCBUF2;


    double current_reading = ((double)current_adc*3.3/1024.0*(20.0/22.0));
    on_output_current_reading(current_reading);


    if (network_adc > 0) {
        double network_voltage = -15.0*((double)network_adc*3.3/1024.0-1.75);
        on_network_voltage_reading(network_voltage);
    }

    if (output_adc > 0) {
        double output_voltage = (double)output_adc*3.3/1024.0*(6.2);
        on_output_voltage_reading(output_voltage);
    }

    if (phone_adc > 0) {
        double phone_voltage = (double)phone_adc*3.3/1024.0*(3.7);
        on_phone_voltage_reading(phone_voltage);
    }

    IFS6bits.ADCP1IF = 0; //clear interrupt flag
    ADSTATbits.P1RDY = 0; //Clear ready bit

}



