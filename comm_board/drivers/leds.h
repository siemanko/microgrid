#ifndef DRIVER_LEDS_H
#define DRIVER_LEDS_H

typedef enum {
    LED_TYPE_GREEN = 0,
    LED_TYPE_YELLOW = 1,
    LED_TYPE_RED = 2,
            
    LED_TYPE_TOTAL,
} LedType;

typedef enum {
    LED_STATE_OFF = 0,
    LED_STATE_ON = 1,
} LedState;

void init_leds();

void leds_set(LedType led_type, LedState state);

// one blink lasts 200ms
// leds are guaranteed to return to previous state after blinking.
void leds_blink(int times);

#endif