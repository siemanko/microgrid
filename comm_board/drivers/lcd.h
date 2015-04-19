/*
 * File:   lcd.h
 * Author: sidor
 *
 * Created on December 9, 2014, 12:10 AM
 */

#ifndef LCD_H
#define	LCD_H

#include <p33EP512GM710.h>
#include <stdint.h>

/* 
  This file defines interface for LCD screen
  
  The wiring diagram is inside the lcd.c file.
  
  Here's minimal code sample to run this:
  
            void init(void) {
                init_board();

                init_timer();

                LCD_init();
            }

            int main() {
                init();

                LCD_set_custom_char_map(hindi_chars);

                while(1) {
                    LCD_reset();
                    LCD_print("hello");
                    delay_ms(1000);
                    LCD_reset();
                    LCD_print_custom(energy_hindi);
                    delay_ms(1000);
                }
                return 1;
            }
 * 
 * 
 */



typedef enum lcd_row_dec {
    LCD_ROW_TOP=0,
    LCD_ROW_BOTTOM=1
} lcd_row;

typedef enum lcd_direction_dec {
    LCD_DIRECTION_LEFT=0,
    LCD_DIRECTION_RIGHT=1
} lcd_direction;


/////////////////////// BASIC API //////////////////////////////////////////////

// Must be called before everything else
// Takes a long time! (about 100ms).
void LCD_init(void);

// clear display
void LCD_reset(void);

// move cursor to beginning of the row.
void LCD_home(lcd_row row);

// Possible inputs are LCD_DIRECTION_{LEFT,RIGHT}
void LCD_move_cursor(lcd_direction which_way);

// write a single character to screen
void LCD_char(unsigned char ch);

// print a string at current cursor position
void LCD_print(char letters[]);


/////////////////////// PRINTING VARIOUS OBJECTS ///////////////////////////////

// Cleans display row and prints a string.
// Possible values are LCD_ROW_{TOP,BOTTOM}
void LCD_replace_row(char letters[], lcd_row row);

// blink a message on some row for some time and then restore previous state.
// Possible row values are LCD_ROW_{TOP,BOTTOM}
void LCD_blink(char * msg, lcd_row row, int wait_ms);

// if depending on "on" value ensures cursor is on or off
// notice that cursor might be behind your current row
void LCD_cursor(int on);

// Output a number at current cursor position.
void LCD_int(uint32_t number);

// Output a number at current cursor position as hex.
void LCD_hex(unsigned int number);

// Output a flow with some number of digits after dot
void LCD_float(float f, int digits_after_dot);


/////////////////////// CUSTOM_CHARACTERS //////////////////////////////////////

// WARNING: THERE CAN BE AT MOST 8 DIFFERENT CUSTOM CHARACTERS 
// USED AT THE SAME TIME.

// Set a map of custom characters.
void LCD_set_custom_char_map(unsigned char *custom_chars);

// Print custom character with a given number.
void LCD_char_custom(unsigned int char_no);

// Print a string of custom characters
void LCD_print_custom(unsigned int* letters);
#endif	/* LCD_H */
