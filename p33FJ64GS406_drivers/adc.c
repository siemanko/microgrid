#include <p33FJ64GS406.h>
#include <stdint.h>


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
