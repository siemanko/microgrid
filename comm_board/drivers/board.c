#include "board.h"


// Internal FRC Oscillator
_FOSCSEL(FNOSC_FRCPLL);  // FRC Oscillator
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_NONE);   // Clock Switching is enabled and Fail Safe Clock Monitor is disabled
						     // OSC2 Pin Function: OSC2 is Clock Output
                                                     // Primary Oscillator Mode: Disabled
_FWDT(FWDTEN_OFF);              // Watchdog Timer Enabled/disabled by user software
_FGS(GWRP_OFF); // General segment Write protect turned off
_FICD(ICS_PGD2 & JTAGEN_OFF) // Select Debug/Program Lines and JTAG OFF

#define SYS_FREQUENCY 36850000

static void init_pins(void) {
    // auxilary output for testing
    TRIS_AUX = TRIS_OUTPUT;
    TRIS_AUX2 = TRIS_OUTPUT;
    TRIS_AUX3 = TRIS_OUTPUT;

    // LCD
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

static void init_oscillator(){
    // setup internal clock for 70MHz/35MIPS
    // 7.37/2=3.685*43=158.455/2=79.2275
    CLKDIVbits.PLLPRE=0;        // PLLPRE (N2) 0=/2
    //PLLFBD=41;                  // pll multiplier (M) = +2
    //PLLFBD = 41;  //PUT BACK TO 41!

    PLLFBD = 38;  //PUT BACK TO 41
    //20 is good value
    CLKDIVbits.PLLPOST=0;       // PLLPOST (N1) 0=/2 
    RCONbits.SWDTEN = 0;
    while (!OSCCONbits.LOCK);
    // For some reason we need a delay here.
    // TODO(szymon): understand why.
    long long i;
    for(i=0; i<1000000LL; ++i);
}

void init_board() {
    init_oscillator();
    init_pins();
}