#include <p33EP512GM710.h>

#include "LibraryFunctions.h"
#include "stdint.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>

float BytesToFloat(uint8_t raw[]){
    // either
    union {
        uint8_t bytes[4];
        float fp;
    } un;
    memcpy(un.bytes, raw, 4);
    return un.fp;
}


void IntToBytes(int source, uint8_t* destination) {
    destination[0] = source & 0xFF;
    destination[1] = (source>>8) & 0xFF;
}

int BytesToInt(uint8_t* raw){
    return (uint16_t)raw[0] + ((uint16_t)(raw[1])<<8);
}

void LongToBytes(uint32_t source, uint8_t* destination) {
    destination[0] = source & 0xFF;
    destination[1] = (source>>8) & 0xFF;
    destination[2] = (source>>16) & 0xFF;
    destination[3] = (source>>24) & 0xFF;

}

uint32_t BytesToLong(uint8_t* raw) {
    uint32_t res = (uint32_t)raw[0] +
               ((uint32_t)raw[1]<<8) +
               ((uint32_t)raw[2]<<16) +
               ((uint32_t)raw[3]<<24);
    return res;
}


void initRandomNumberGen(){
    srand(time(NULL));
}

uint16_t getRandomInt(){
    return rand();
}

int getRandomIntWithBounds(int lb, int ub){
    int randomNum=0;

    return randomNum;
}


void blink_led(void) {
    TRISEbits.TRISE0 = 0;

    long i;
    int x = 2;
    while(x--) {
        LATEbits.LATE0 = 1;
        for (i = 50000L; i>=0; --i);
        LATEbits.LATE0 = 0;
        for (i = 50000L; i>=0; --i);
    }
    for (i = 500000L; i>=0; --i);

}

void slow_blink(void) {
    int old = TRISEbits.TRISE0;
    TRISEbits.TRISE0 = 0;

    LATEbits.LATE0 = 1;
    long i;
    for (i = 1000000L; i>=0; --i);
    LATEbits.LATE0 = 0;
    for (i = 1000000L; i>=0; --i);

    TRISEbits.TRISE0 = old;
}