#ifndef DRIVERS_TIMER_H
#define	DRIVERS_TIMER_H

#include <stdint.h>
#include <time.h>

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

// WARNING: this stopwatch is primarily meant for chrome
// don't use! (ask szymon if you need a timer - at the time
// of writing there's still plenty unused).
void stopwatch_start();
// Returns number of us since the last time stopwatch was called.
uint32_t stopwatch_stop_us();

// Number of milliseconds passed in current second.
uint16_t time_milliseconds();

// Time since 1 Jan 1970 in seconds.
uint32_t time_seconds_since_epoch();

// Time since 1 Jan 1970 in milliseconds.
uint64_t time_milliseconds_since_epoch();

// Time travel.
void time_set_seconds_since_epoch(uint32_t value);

// Returns a data structure representing current time.
struct tm* ulink_local_time();

#endif	/* DELAYS_H */

