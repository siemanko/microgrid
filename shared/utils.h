#ifndef SHARED_UTILS_H
#define	SHARED_UTILS_H

#include <stdint.h>
#include <stdarg.h>

#define assert(condition) internal_assert((condition), "%s (%s:%d)", ( #condition ) , __FILE__, __LINE__)
#define safe_malloc(size) internal_safe_malloc((size), "malloc (%s:%d)", __FILE__, __LINE__)

float bytes_to_float(uint8_t raw[]);

void float_to_bytes(float source, uint8_t* destination);


void int_to_bytes(int source, uint8_t* destination);

int bytes_to_int(uint8_t* source);

void uint32_to_bytes(uint32_t source, uint8_t* destination);

uint32_t bytes_to_uint32(uint8_t* source);

double double_abs(double a);

double double_max(double a, double b);

void init_random();

uint16_t get_random_int();

char* copy_string(const char*);

char* copy_array(const char*, int length);

// compares first <prefix> bytes of two strings
// returns 1 if equal zero otherwise.
int str_prefix_equal(char* s1, char* s2, int prefix);

void internal_assert(int condition, const char* format, ...);

void* internal_safe_malloc(uint16_t size, const char* format, ...);

void set_assert_callback(void (*callback)(const char*, va_list));


// Initial should normally be zero, but
// can be an output of another checksum call
// to computer joint checksum for multiple pieces of data.
uint32_t checksum(uint32_t initial, const uint8_t* data, int len);

#endif	/* LIBRARYFUNCTIONS_H */

