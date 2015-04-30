#ifndef USER_INTERFACE_BALANCE_H
#define USER_INTERFACE_BALANCE_H

#include <stdint.h>

uint32_t balance_get();
void balance_set(uint32_t value);
void balance_step();

#endif