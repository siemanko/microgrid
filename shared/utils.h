#include "stdint.h"

#ifndef SHARED_UTILS_H
#define	SHARED_UTILS_H

float bytes_to_float(uint8_t raw[]);

void int_to_bytes(int source, uint8_t* destination);

int bytes_to_int(uint8_t* source);

void uint32_to_bytes(uint32_t source, uint8_t* destination);

uint32_t bytes_to_long(uint8_t* source);

void init_random();

uint16_t get_random_int();

char* copy_string(const char*);

char* copy_array(const char*, int length);

// compares first <prefix> bytes of two strings
// returns 1 if equal zero otherwise.
int str_prefix_equal(char* s1, char* s2, int prefix);

#endif	/* LIBRARYFUNCTIONS_H */

