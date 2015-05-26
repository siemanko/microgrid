#include "leds.h"

#include <p33EP512GM710.h>

#include "drivers/timer.h"
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

static int leds_states[LED_TYPE_TOTAL];

void init_leds() {
    // set the pins as output pins
    TRIS_LED_GREEN             =  0;
    TRIS_LED_YELLOW            =  0;
    TRIS_LED_RED               =  0;
    // make the simulated ground do it's job
    
    // switch the leds off initially.
    leds_set(LED_TYPE_GREEN,  LED_STATE_OFF);
    leds_set(LED_TYPE_YELLOW, LED_STATE_OFF);
    leds_set(LED_TYPE_RED,    LED_STATE_OFF);
}

void leds_set(LedType led_type, LedState state) {
    leds_states[led_type] = state;
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

static void flip_leds() {
    int lidx;
    for (lidx=0; lidx < LED_TYPE_TOTAL; ++lidx) {
        leds_set(lidx, 1 - leds_states[lidx]);
    }
}

void leds_blink(int times) {
    while(times--) {
        flip_leds();
        delay_ms(100);
        flip_leds();
        delay_ms(100);
    }
}