#include "button.h"
#include <stdio.h>
#include "../user_interface/display.h"
#include <p33EP512GM710.h>
#include "leds.h"

#define BUTTON_NUM_CHECKS_BEFORE_ACTIVE 1    //DS:  Edit, Szymon's code was 50

#define TRIS_BUTTON  TRISAbits.TRISA4
#define PIN_BUTTON   LATAbits.LATA4

static int num_times_active = 0;

void init_button() {
    // set button as input
    TRIS_BUTTON = 1;
    
    //Interrupt for handling button press
    IFS1bits.CNIF = 0;      //Change notification in input flag set to zero
    IPC4bits.CNIP0 = 1;     //Interrupt priority
    IEC1bits.CNIE = 1;      //CNI interrupt enable   
    CNENAbits.CNIEA4 = 1;   //Enable for A4

}

// returns true if button was recently pressed.
int button_check() {
    return num_times_active >= BUTTON_NUM_CHECKS_BEFORE_ACTIVE;
}

void button_reset() {
    num_times_active = 0;  
}

void button_step() {
    if (num_times_active < BUTTON_NUM_CHECKS_BEFORE_ACTIVE) {
        if (!PORTAbits.RA4) {
            ++num_times_active;
        } else {
            num_times_active = 0;
        }
    }
}

void display_button_status(){     
    char outBuf[10] ="";  
    sprintf(outBuf,"bp is: %d ", num_times_active);
    LCD_replace_row(outBuf, LCD_ROW_TOP);
}


void __attribute__((interrupt, no_auto_psv)) _CNInterrupt( void ){
    IFS1bits.CNIF = 0;      //Change notification in input flag set to zero
    num_times_active=1;
}

