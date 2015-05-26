#include "leds.h"

#include <p33EP512GM710.h>

#include "shared/utils.h"

/* If you can read MIT logo,
   those live in the 6 pin header 
   on the right hand side
 those are from top to bottom:
    E1 and E0
    C3
    A9
    A4
    GND
    SOMETHING ODD
 */

#define TRIS_LED_GREEN             TRISEbits.TRISE1
#define TRIS_LED_YELLOW            TRISCbits.TRISC3
#define TRIS_LED_RED               TRISAbits.TRISA9

#define PIN_LED_GREEN              LATEbits.LATE1
#define PIN_LED_YELLOW             LATCbits.LATC3
#define PIN_LED_RED                LATAbits.LATA9

void init_leds() {
    // set the pins as output pins
    TRIS_LED_GREEN             =  0;
    TRIS_LED_YELLOW            =  0;
    TRIS_LED_RED               =  0;
    // make the simulated ground do it's job
    
    // switch the leds off initially.
    led_sets(LED_TYPE_GREEN,  LED_STATE_OFF);
    led_sets(LED_TYPE_YELLOW, LED_STATE_OFF);
    led_sets(LED_TYPE_RED,    LED_STATE_OFF);
}

void led_sets(LedType led_type, LedState state) {
    if (led_type == LED_TYPE_GREEN) {
        PIN_LED_GREEN = state;
    } else if (led_type == LED_TYPE_YELLOW) {
        PIN_LED_YELLOW = state;
    } else if (led_type == LED_TYPE_RED) {
        PIN_LED_RED = state;
    } else {
        assert(0);
    }
}