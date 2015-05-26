#ifndef DRIVER_LEDS_H
#define DRIVER_LEDS_H

typedef enum {
    LED_TYPE_GREEN = 1,
    LED_TYPE_YELLOW = 2,
    LED_TYPE_RED = 3,
} LedType;

typedef enum {
    LED_STATE_OFF = 0,
    LED_STATE_ON = 1,
} LedState;

void init_leds();

void led_sets(LedType led_type, LedState state);


#endif