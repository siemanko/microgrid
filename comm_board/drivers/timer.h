#ifndef DRIVERS_TIMER_H
#define	DRIVERS_TIMER_H

#include <stdint.h>

// Must be called before using timer.
void init_timer(void);


// WARNING: currently timer loops every 122 s.
// Delays longer than that are not supported

// Wait given number of cycles.
void delay_cycles(uint32_t cycles);

// Wait given number of milliseconds.
void delay_ms(uint32_t ms);

// Wait given number of microseconds.
void delay_us(uint32_t us);

// Wait given number of nanoseconds.
// Accuracy is +/- 14 ns.
void delay_ns(uint32_t ns);

// Number of milliseconds passed in current second.
uint16_t time_milliseconds();

// Time since 1 Jan 1970 in seconds.
uint32_t time_seconds_since_epoch();

// Time since 1 Jan 1970 in milliseconds.
uint64_t time_milliseconds_since_epoch();
#endif	/* DELAYS_H */

