/* 
 * File:   LCD_custom_chars.h
 * Author: sidor
 *
 * Created on January 17, 2015, 9:51 AM
 */

#ifndef USER_INTERFACE_HINDI_H
#define	USER_INTERFACE_HINDI_H


unsigned char hindi_chars[] = {
    // android (0)
    0b01110,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    0b01110,
    0b01110,
    0b01110,
    // dick (1)
    0b01110,
    0b01110,
    0b01110,
    0b01110,
    0b01110,
    0b11111,
    0b11111,
    0b11111,
    // stripe (2)
    0b00000,
    0b00000,
    0b01110,
    0b01110,
    0b01110,
    0b01110,
    0b00000,
    0b00000,
    // energy1 (3)
    0b00000,
    0b11111,
    0b01010,
    0b01010,
    0b01000,
    0b00100,
    0b00010,
    0b00001,
    // energy2 (4)
    0b00000,
    0b11111,
    0b00001,
    0b00001,
    0b01111,
    0b10101,
    0b01001,
    0b00001,
    // energy3 (5)
    0b00101,
    0b11111,
    0b00101,
    0b00101,
    0b11101,
    0b01101,
    0b10101,
    0b00101,
    // clock(6)
    0b00000,
    0b01110,
    0b10101,
    0b10101,
    0b11001,
    0b10001,
    0b01110,
    0b00000
};

unsigned int energy_hindi[] = {3,4,5,0};


#endif	/* LCD_CUSTOM_CHARS_H */

