#include "pwm.h"

#include <p33FJ64GS406.h>


void init_PWM()
{
    PTPER = 48100;             				/* PTPER = 1/(500kHz*1.04ns) = 1923, where 500 khz is freq
							of the PWM  and 1.04ns is PWM resolution. */
    /* ~~~~~~~~~~~~~~~~~~~~~~ PWM1 Configuration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
  //  IOCON3bits.PENH = 1;   					/* PWM1H is controlled by PWM module */
    IOCON4bits.PENL = 1;   					/* PWM1L is controlled by PWM module */
    IOCON4bits.PMOD = 0;   					/* Select Complementary Output PWM mode */
    IOCON4bits.SWAP = 1;

    IOCON3bits.PENH = 1;
    IOCON3bits.PMOD = 3;
    IOCON3bits.SWAP = 0;
    
    PDC3= 0.1*PTPER;
    DTR3 = 30;
    ALTDTR3 = 30;
    PHASE3 = 0; 
       
    
    PDC4 = 0.1*PTPER;                 			/* PDC = (PTPER / DutyCycle)  */
    DTR4    = 30;               			/* Deadtime = (80ns / 1.04ns) where 65ns is desired deadtime */
    ALTDTR4 = 30;    						/* ALTDeadtime = (65ns / 1.04ns) where 65ns is desired deadtime */
    PHASE4 = 0;     			            /* No phase shift */
    

    // TRGCON4bits.TRGDIV = 1000;       // Trigger interrupt generated once every 15 PWM cycle
    // TRGCON4bits.TRGSTRT = 1;      // Trigger generated after waiting 0 PWM cycles

    PTCONbits.PTEN = 1;			   			/* Enable the PWM Module */

}