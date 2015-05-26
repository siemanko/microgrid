#include "pwm.h"

#include "p33FJ64GS406.h"


void init_PWM()
{
    PTPER = 1923;             				/* PTPER = 1/(500kHz*1.04ns) = 1923, where 500 khz is freq
							of the PWM  and 1.04ns is PWM resolution. */
    /* ~~~~~~~~~~~~~~~~~~~~~~ PWM1 Configuration ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
    IOCON3bits.PENH = 1;   					/* PWM1H is controlled by PWM module */
    IOCON3bits.PENL = 1;   					/* PWM1L is controlled by PWM module */
    IOCON3bits.PMOD = 0;   					/* Select Complementary Output PWM mode */
    IOCON3bits.SWAP = 1;

    PDC3 = 900;                 			/* PDC = (PTPER / DutyCycle)  */
    DTR3    = 20;               			/* Deadtime = (80ns / 1.04ns) where 65ns is desired deadtime */
    ALTDTR3 = 30;    						/* ALTDeadtime = (65ns / 1.04ns) where 65ns is desired deadtime */
    PHASE3 = 0;     			            /* No phase shift */

    TRGCON3bits.TRGDIV  =15;       // Trigger interrupt generated once every 15 PWM cycle
    TRGCON3bits.TRGSTRT = 1;      // Trigger generated after waiting 0 PWM cycles

    PTCONbits.PTEN = 1;			   			/* Enable the PWM Module */
}
