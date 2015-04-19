#ifndef DRIVERS_BOARD_H
#define	DRIVERS_BOARD_H

#include <p33EP512GM710.h>

#define TRIS_INPUT 1
#define TRIS_OUTPUT 0

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

/* How big are different types?
 *
 * short 2
 * int 2
 * long 4
 * long long 8
 */


void init_board();

#endif