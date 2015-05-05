#include <p33FJ64GS406.h>
#include <stdint.h>

#include "reading_model.h"


void init_ADC(int trigger) {
    ADCONbits.FORM    = 0;       // Integer data format
    ADCONbits.EIE     = 0;       // Early Interrupt disabled
    ADCONbits.ORDER   = 0;       // Convert even channel first
    ADCONbits.SEQSAMP = 0;       // Select simultaneous sampling
    ADCONbits.ADCS    = 5;       // ADC clock = FADC/6 = 120MHz / 6 = 20MHz,
				 // 12*Tad = 1.6 MSPS

    //IFS6bits.ADCP0IF  = 0;	 // Clear ADC interrupt flag pair 0
    //IPC27bits.ADCP0IP = 3;	 // Set ADC interrupt priority pair 0
    //IEC6bits.ADCP0IE  = 0;       // Enable the ADC pair 0 interrupt

    IFS6bits.ADCP1IF  = 0;	 // Clear ADC interrupt flag pair 1
    IPC27bits.ADCP1IP = 3;	 // Set ADC interrupt priority pair 1
    IEC6bits.ADCP1IE  = 1;       // Enable the ADC pair 1 interrupt

    ADPCFGbits.PCFG0 = 0; 	 // Primary Voltage
    ADPCFGbits.PCFG1 = 0; 	 // 5 volts
    ADPCFGbits.PCFG2 = 0; 	 // 12 volts out
    ADPCFGbits.PCFG3 = 0;         //current out

    //ADSTATbits.P0RDY   = 0; 	 // Clear Pair 0 data ready bit
    //ADCPC0bits.IRQEN0  = 1;      // Enable ADC Interrupt
    ADCPC0bits.TRGSRC0 = trigger; 	 // ADC Pair 0 triggered by PWM4

    ADSTATbits.P1RDY   = 0; 	 // Clear Pair 1 data ready bit
    ADCPC0bits.IRQEN1  = 1;      // Enable ADC Interrupt for pair 1
    ADCPC0bits.TRGSRC1 = trigger; 	 // ADC Pair 1 triggered by timer1

    ADCONbits.ADON = 1;		 // Enable the ADC Module early for ADC Settling Time
}

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
        double network_voltage = -15.0*((double)network_adc*3.3/1024.0-1.75)+30.0;
        on_network_voltage_reading(network_voltage);
    }

    if (output_adc > 0) {
        double output_voltage = (double)output_adc*3.3/1024.0*(7.2);
        on_output_voltage_reading(output_voltage);
    }

    if (phone_adc > 0) {
        double phone_voltage = (double)phone_adc*3.3/1024.0*(69.0/22.0);
        on_phone_voltage_reading(phone_voltage);
    }

    IFS6bits.ADCP1IF = 0; //clear interrupt flag
    ADSTATbits.P1RDY = 0; //Clear ready bit

}




