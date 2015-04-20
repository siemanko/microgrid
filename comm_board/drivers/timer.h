#ifndef DRIVERS_TIMER_H
#define	DRIVERS_TIMER_H


#define CYCLES_PER_SECOND 35000000L

// WARNING: currently timer loops every 122 s.
// Absolute time is a TODO and requires interrupts.

// Must be called before using timer.
void init_timer(void);

// Resets timer to zero
inline void reset_time();

// Returns number of cycles since last reset
inline long timer_cycles(void);

// WARNING: As of no methods below reset the clock.
// For some high delays there can be overflow errors.
// This will be fixed later with absolute time.

// Wait given number of cycles.
void delay_cycles(long cycles);

// Wait given number of milliseconds.
void delay_ms(long ms);

// Wait given number of microseconds.
void delay_us(long us);

// Wait given number of nanoseconds.
// Accuracy is +/- 14 ns.
void delay_ns(long ns);

#endif	/* DELAYS_H */

