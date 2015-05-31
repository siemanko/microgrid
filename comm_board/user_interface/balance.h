#ifndef USER_INTERFACE_BALANCE_H
#define USER_INTERFACE_BALANCE_H

#include <stdint.h>

extern int balance_update_hours;
extern int balance_update_minutes;
extern uint32_t balance_update_ammount;

uint32_t balance_get();
void balance_set(uint32_t value);
void balance_step();
int32_t balance_estimated_time_remaining();

#endif