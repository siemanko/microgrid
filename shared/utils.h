#include "stdint.h"

#ifndef SHARED_UTILS_H
#define	SHARED_UTILS_H

float bytes_to_float(uint8_t raw[]);

void int_to_bytes(int source, uint8_t* destination);

int bytes_to_int(uint8_t* source);

void long_to_bytes(uint32_t source, uint8_t* destination);

uint32_t bytes_to_long(uint8_t* source);

void blink_led(void);

void slow_blink(void);

void init_random();

uint16_t get_random_int();

char* copy_string(const char*);

char* copy_array(const char*, int length);

#endif	/* LIBRARYFUNCTIONS_H */

