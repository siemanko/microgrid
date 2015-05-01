#include "timer.h"

#include <p33FJ64GS406.h>

void init_timer(){
    T1CONbits.TCKPS1 = 0;      //Timer1 Input Clock Prescale Select bits
    T1CONbits.TCKPS0 = 1;       // 01= divid by 8
                                // 11 = divide by 256
    T1CONbits.TCS = 0;          //Use Internal clock
    T1CONbits.TGATE = 0;      //Gated time accumulation disabled
    PR1 = 512;
    T1CONbits.TON = 1;         //Starts 16-bit Timer1
}