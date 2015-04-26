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


void int_to_bytes(int source, uint8_t* destination) {
    destination[0] = source & 0xFF;
    destination[1] = (source>>8) & 0xFF;
}

int bytes_to_int(uint8_t* raw){
    return (uint16_t)raw[0] + ((uint16_t)(raw[1])<<8);
}

void long_to_bytes(uint32_t source, uint8_t* destination) {
    destination[0] = source & 0xFF;
    destination[1] = (source>>8) & 0xFF;
    destination[2] = (source>>16) & 0xFF;
    destination[3] = (source>>24) & 0xFF;

}

uint32_t bytes_to_long(uint8_t* raw) {
    uint32_t res = (uint32_t)raw[0] +
               ((uint32_t)raw[1]<<8) +
               ((uint32_t)raw[2]<<16) +
               ((uint32_t)raw[3]<<24);
    return res;
}


void init_random(){
    srand(time(NULL));
}

uint16_t get_random_int(){
    return rand();
}