#include "board.h"

#include <p33FJ64GS406.h>

/* Configuration Bit Settings */
_FBS(BWRP_WRPROTECT_OFF);
_FGS(GSS_OFF&GCP_OFF&GWRP_OFF);
_FOSCSEL(FNOSC_FRC)
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE ) //it was CSECMDandON
_FWDT(FWDTEN_OFF)
_FPOR(FPWRT_PWR128 )
_FICD(ICS_PGD2 & JTAGEN_OFF)

void init_oscillator(void) {
    /* Configure Oscillator to operate the device at 40Mhz
       Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
       Fosc= 7.37*(43)/(2*2)=80Mhz for Fosc, Fcy = 40Mhz */

    /* Configure PLL prescaler, PLL postscaler, PLL divisor */
    PLLFBD=41; 				/* M = PLLFBD + 2 */
    CLKDIVbits.PLLPOST=0;   /* N1 = 2 */
    CLKDIVbits.PLLPRE=0;    /* N2 = 2 */

    __builtin_write_OSCCONH(0x01);			/* New Oscillator FRC w/ PLL */
    __builtin_write_OSCCONL(0x01);  		/* Enable Switch */

    while(OSCCONbits.COSC != 0b001);		/* Wait for new Oscillator to become FRC w/ PLL */
    while(OSCCONbits.LOCK != 1);			/* Wait for Pll to Lock */

    /* Now setup the ADC and PWM clock for 120MHz
       ((FRC * 16) / APSTSCLR ) = (7.37 * 16) / 1 = ~ 120MHz*/

    //delayDebug(200);

    ACLKCONbits.FRCSEL = 1;				/* FRC provides input for Auxiliary PLL (x16) */
    ACLKCONbits.SELACLK = 1;				/* Auxiliary Oscillator provides clock source for PWM & ADC */
    ACLKCONbits.APSTSCLR = 7;				/* Divide Auxiliary clock by 1 */
    ACLKCONbits.ENAPLL = 1;                             /* Enable Auxiliary PLL */

    while(ACLKCONbits.APLLCK != 1);			/* Wait for Auxiliary PLL to Lock */

}

void init_board() {
    init_oscillator();
}
