#ifndef UTILS_CRON_H
#define UTILS_CRON_H

#include <stdint.h>

#include <constants.h>

void init_cron();

#ifdef CRON_STATS
    #define cron_repeat_rapidly(callback) cron_repeat_rapidly_internal((callback), #callback)
    #define cron_repeat_every_s(num_s, callback) cron_repeat_every_s_internal(num_s, (callback), #callback)
    #define cron_repeat_every_ms(num_ms, callback) cron_repeat_every_ms_internal(num_ms, (callback), #callback)
#else
    #define cron_repeat_rapidly(callback) cron_repeat_rapidly_internal((callback), 0)
    #define cron_repeat_every_s(num_s, callback) cron_repeat_every_s_internal(num_s, (callback), 0)
    #define cron_repeat_every_ms(num_ms, callback) cron_repeat_every_ms_internal(num_ms, (callback), 0)
#endif

void cron_repeat_rapidly_internal(void (*callback)(), char* name);

// This one is recommended over repeat_every_ms, as it consumes
// less memory.
void cron_repeat_every_s_internal(uint32_t num_s, void (*callback)(), char* name);

void cron_repeat_every_ms_internal(uint32_t num_ms, void(*callback)(), char* name);

void cron_reset_timers();

void cron_step();

void cron_print_stats();

#endif