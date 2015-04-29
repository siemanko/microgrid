#include "timer.h"

#include <p33EP512GM710.h>

#define CYCLES_PER_SECOND 35000000L

void init_timer(void) {
    // Set up 32 bit clock for high resolution delays.
    T8CONbits.TON = 0;       // Disable Timer
    T8CONbits.TCS = 0;       // Select internal instruction cycle clock
    T8CONbits.TGATE = 0;     // Disable Gated Timer mode
    T8CONbits.TCKPS = 0b000; // Select 1:1 Prescaler
    T8CONbits.T32 = 1;       // Enable 32 bit mode
    T8CONbits.TSIDL = 0;     // tick when idle
    T9CONbits.TSIDL = 0;     // ditto
    TMR9HLD = 0x00;          // reset timer register
    TMR8 = 0x00;             // ditto
    T8CONbits.TON = 1; // Start Timer
    
    
    // Set up 32 bit clock for stopwatch
    T6CONbits.TON = 0;       // Disable Timer
    T6CONbits.TCS = 0;       // Select internal instruction cycle clock
    T6CONbits.TGATE = 0;     // Disable Gated Timer mode
    T6CONbits.TCKPS = 0b000; // Select 1:1 Prescaler
    T6CONbits.T32 = 1;       // Enable 32 bit mode
    T6CONbits.TSIDL = 0;     // tick when idle
    T7CONbits.TSIDL = 0;     // ditto
    TMR7HLD = 0x00;          // reset timer register
    TMR6 = 0x00;             // ditto
    T6CONbits.TON = 1; // Start Timer
    
    
    // Setup absolute time clock
    // EXPLENATION FOR RESET/INTERRUPT PERIOD
    // SYS_FREQUENCY / (PRESCALER * MILLISECONDS_IN_SECONDS)
    // 36850000 / (256 * 1000)
    PR1 = 144;

    T1CONbits.TON = 1;
    T1CONbits.TCKPS = 0b11; // PRESCALER: 256

    IPC0bits.T1IP = 6;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
}


///////////////////////////// DELAYS ////////////////////////////////////////

// Inline to make next cycle be time zero.
inline void reset_time() {
    TMR9HLD = 0x00;
    TMR8 =    0x00;
}

inline uint32_t timer_cycles(void) {
     uint32_t lsw = TMR8;
     uint32_t msw = TMR9HLD;
     return msw*((uint32_t)1<<(uint32_t)16) + lsw;
}

void delay_cycles(uint32_t cycles) {
    reset_time();
    // ideally one would subtract execution of get_time()...
    while(timer_cycles()<cycles);
}


void delay_ms(uint32_t ms) {
    delay_cycles(ms * (CYCLES_PER_SECOND/1000L));
}

void delay_us(uint32_t us) {
    delay_cycles(us * (CYCLES_PER_SECOND/1000000L));
}

void delay_ns(uint32_t ns) {
    // This funny formula tries to avoid overflow for large number of ns.
    delay_cycles((ns * (CYCLES_PER_SECOND/1000000L))/1000L );
}

void stopwatch_start() {
    TMR7HLD = 0x00;
    TMR6 =    0x00;    
}

uint32_t stopwatch_stop_us() {
    uint32_t lsw = TMR6;
    uint32_t msw = TMR7HLD;
    uint32_t stopwatch_cycles = msw*((uint32_t)1<<(uint32_t)16) + lsw;
    // derivation: 1000000 us * stopwatch_cycles / CYCLES_PER_SECOND
    return stopwatch_cycles / 35;
}


///////////////////////// ABSOLUTE TIME //////////////////////////////////

static uint32_t abs_time_s = 0;
static uint16_t abs_time_ms = 0;

void __attribute__((__interrupt__,auto_psv)) _T1Interrupt(void){
    ++abs_time_ms;
    if (abs_time_ms == 1000) {
        abs_time_ms = 0;
        ++abs_time_s;
    }
    IFS0bits.T1IF = 0;    
}

uint16_t time_milliseconds() {
    return abs_time_ms;
}

uint32_t time_seconds_since_epoch() {
    return abs_time_s;
}

uint64_t time_milliseconds_since_epoch() {
    return (uint64_t)abs_time_s * 1000 + abs_time_ms;
}

void time_set_seconds_since_epoch(uint32_t value) {
    abs_time_s = value;
}