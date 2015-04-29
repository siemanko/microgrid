#ifndef UTILS_CRON_H
#define UTILS_CRON_H

#include <stdint.h>

void init_cron();

void cron_repeat_rapidly(void (*callback)());

// This one is recommended over repeat_every_ms, as it consumes
// less memory.
void cron_repeat_every_s(uint32_t num_s, void (*callback)());

void cron_repeat_every_ms(uint32_t num_ms, void(*callback)());

void cron_reset_timers();

void cron_step();

#endif