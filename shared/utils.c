#include "utils.h"

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

float bytes_to_float(uint8_t raw[]){
    // either
    union {
        uint8_t bytes[4];
        float fp;
    } un;
    memcpy(un.bytes, raw, 4);
    return un.fp;
}

void float_to_bytes(float source, uint8_t* destination) {
    union {
        uint8_t bytes[4];
        float fp;
    } un;
    un.fp = source;
    memcpy(destination, un.bytes, 4);
}


void int_to_bytes(int source, uint8_t* destination) {
    destination[0] = source & 0xFF;
    destination[1] = (source>>8) & 0xFF;
}

int bytes_to_int(uint8_t* raw){
    return (uint16_t)raw[0] + ((uint16_t)(raw[1])<<8);
}

void uint32_to_bytes(uint32_t source, uint8_t* destination) {
    destination[0] = source & 0xFF;
    destination[1] = (source>>8) & 0xFF;
    destination[2] = (source>>16) & 0xFF;
    destination[3] = (source>>24) & 0xFF;

}

uint32_t bytes_to_uint32(uint8_t* raw) {
    uint32_t res = (uint32_t)raw[0] +
               ((uint32_t)raw[1]<<8) +
               ((uint32_t)raw[2]<<16) +
               ((uint32_t)raw[3]<<24);
    return res;
}

double double_abs(double a) {
    if ( a >= 0.0) return a;
    else return -a;
}

double double_max(double a, double b) {
    if (a>b) return a;
    else return b;
}

void init_random(){
    srand(time(NULL));
}

uint16_t get_random_int(){
    return rand();
}

char* copy_string(const char* input) {
    int length = strlen(input);
    char* ret = safe_malloc(sizeof(char)*(length + 1));
    int i;
    for(i=0; i<length+1; ++i) {
        ret[i] = input[i];
    }
    return ret;
}

char* copy_array(const char* input, int length) {
    char* ret = safe_malloc(sizeof(char)*length);
    int i;
    for(i=0; i<length; ++i) {
        ret[i] = input[i];
    }
    return ret;
}

int str_prefix_equal(char* s1, char* s2, int prefix) {
    int i;
    for(i=0; i<prefix; ++i) {
        if (s1[i] != s2[i])
            break;
    }
    return i == prefix;
}

static void (*assert_callback)(char*, va_list);

void internal_assert(int condition, char* format, ...) {
    if (!condition && assert_callback != 0) {
        va_list args;
        va_start(args, format);
        assert_callback(format, args);
        va_end(args);
    }
}

void* internal_safe_malloc(uint16_t size, char* format, ...) {
    void *ret = malloc(size);
    if (ret == 0) {
        va_list args;
        va_start(args, format);
        assert_callback(format, args);
        va_end(args);
        return 0;
    } else {
        return ret;
    }
}

void set_assert_callback(void (*callback)(char*, va_list)) {
    assert_callback = callback;
}

uint32_t checksum(uint32_t initial, const uint8_t* data, int len) {
    int didx;
    for (didx = 0; didx < len; ++didx) {
        initial = initial*333 + data[didx];
    }
    return initial;
}