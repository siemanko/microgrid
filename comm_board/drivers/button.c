#include "button.h"

#include <p33EP512GM710.h>

#define BUTTON_NUM_CHECKS_BEFORE_ACTIVE 50

#define TRIS_BUTTON  TRISAbits.TRISA4
#define PIN_BUTTON   LATAbits.LATA4

int num_times_active = 0;

void init_button() {
    // set button as input
    TRIS_BUTTON = 1;
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