#ifndef PININIT_H
#define	PININIT_H

#include <p33EP512GM710.h>
#define TRIS_INPUT 1
#define TRIS_OUTPUT 0

// send_to_arduino
#define TRIS_AUX TRISAbits.TRISA0
#define TRIS_AUX2 TRISAbits.TRISA11
#define TRIS_AUX3 TRISAbits.TRISA12

#define TRIS_RS TRISEbits.TRISE15
#define TRIS_RW TRISDbits.TRISD5
#define TRIS_EN TRISCbits.TRISC8

#define TRIS_D0 TRISBbits.TRISB10
#define TRIS_D1 TRISBbits.TRISB11
#define TRIS_D2 TRISBbits.TRISB12
#define TRIS_D3 TRISBbits.TRISB13

#define TRIS_D4 TRISAbits.TRISA10
#define TRIS_D5 TRISAbits.TRISA7
#define TRIS_D6 TRISBbits.TRISB14
#define TRIS_D7 TRISBbits.TRISB15

void pininit(void) {
    TRIS_AUX = TRIS_OUTPUT;
    TRIS_AUX2 = TRIS_OUTPUT;
    TRIS_AUX3 = TRIS_OUTPUT;

    TRIS_RS = TRIS_OUTPUT;
    TRIS_RW = TRIS_OUTPUT;
    TRIS_EN = TRIS_OUTPUT;
    TRIS_D0 = TRIS_OUTPUT;
    TRIS_D1 = TRIS_OUTPUT;
    TRIS_D2 = TRIS_OUTPUT;
    TRIS_D3 = TRIS_OUTPUT;
    TRIS_D4 = TRIS_OUTPUT;
    TRIS_D5 = TRIS_OUTPUT;
    TRIS_D6 = TRIS_OUTPUT;
    TRIS_D7 = TRIS_OUTPUT;

    // All pins digital for starts.
    ANSELA = 0; // PORTA pins digital
    ANSELB = 0; // PORTB pins digital
    ANSELC = 0; // PORTC pins digital
    ANSELD = 0; // PORTD pins digital
    ANSELE = 0; // PORTE pins digital
}

#endif