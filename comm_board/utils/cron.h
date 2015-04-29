#ifndef UTILS_CRON_H
#define UTILS_CRON_H

void repeat_rapidly(void (*callback)());

void repeat_every_s(uint32_t num_s, void (*callback)());

void repeat_every_ms(uint16_t num_ms, void(*callback)());

#endif UTILS_CRON_H