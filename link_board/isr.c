#include "isr.h"

#include "p33FJ64GS406.h"
#include <libq.h>
#include <dsp.h>
#include <stdint.h>

#include "ReadingModel.h"
#define refv 2.4f

float kp=0.00021231;
float TsTi=4.2;

float WR0, WR1, WR2, WR3, WR4, WR5, WR6, WR7, WR8, WR9;
float refi;
//float ADC0value;
float ADC1value;
int x=0;


int readings_updated = 0;
uint16_t battery_output_current_adc = 0;
uint16_t battery_input_current_adc = 0;
uint16_t battery_voltage_adc = 0;
uint16_t network_voltage_adc = 0;

void __attribute__((__interrupt__, no_auto_psv)) _ADCP1Interrupt() {
    TRISFbits.TRISF6 = 0;

    LATFbits.LATF6 = 1;
    x=0;
    WR6=WR5;
    WR7= WR9;
    ADC1value= (ADCBUF2*3.3/1024);
    // V_out sense
    // ADCBUF1: Vin_Sense
    // ADCBUF0: Current INTO the battery
    // ADCBUF3: Current from battery to converter
    
    // 0.8 is (1024-775)=248 in de
    // 2.4 is voltage divider
    WR5 = refv - ADC1value; 
   
    WR9=WR7+kp*(WR5-WR6)+kp*TsTi*WR5;
    refi = WR9 * (1923 / 3.3);
    if (refi > 1200) {
        PDC3 = 1200;
        WR9  = 1.8;
    } else {
        PDC3 = refi;
    }

    
    LATFbits.LATF6   = 0;
    IFS6bits.ADCP1IF = 0; //clear interrupt flag   
    ADSTATbits.P1RDY = 0; //Clear ready bit

    battery_output_current_adc   =   ADCBUF3;
    battery_input_current_adc    =   ADCBUF0;
    battery_voltage_adc          =   ADCBUF1;
    network_voltage_adc          =   ADCBUF2;

    readings_updated = 1;

}
