#include "lcd.h"

#include "timer.h"

/* WIRING
 * If we look on the board from the top such that MIT logo is in top-left:
 * D7 D6
 * D5 D4
 * D3 D2
 * D1 D0
 * E R/W
 * RS  +
 *  +  -
 *
 * WARNING: Remeber that LCD requires two negative (one of which is V0 for
 * contrast) and one positive supplies. What you have available on board is
 * not idead (two positives and one negative, so you need to be a little bit
 * creative.
 * 
 * Tack time!
 */

#define RS LATEbits.LATE15
#define RW LATDbits.LATD5
#define EN LATCbits.LATC8

#define D0 LATBbits.LATB10
#define D1 LATBbits.LATB11
#define D2 LATBbits.LATB12
#define D3 LATBbits.LATB13

#define D4 LATAbits.LATA10
#define D5 LATAbits.LATA7
#define D6 LATBbits.LATB14
#define D7 LATBbits.LATB15



static lcd_row current_row;
// current character in row (0-15)
static int current_char;

static char LCD_buffer[32];

static void LCD_command(unsigned char command);
static void LCD_write_info(unsigned char info);

void reset_buffer() {
    int l=0;
    int c=0;
    while(l<2) {
        c=0;
        while(c<16) {
            LCD_buffer[l * 16 + c] = 'X';
            ++c;
        }
        ++l;
    }
}

void init_LCD(void){

    // Set all pins to output and clear them initially
    RW = 0; RS = 0; EN = 0;
    D0 = 0; D1 = 0; D2 = 0; D3 = 0;
    D4 = 0; D5 = 0; D6 = 0; D7 = 0;

    int repeat = 3;
    while(repeat--) {
        // Initialize sequence
        LCD_write_info(0x30);
        EN = 1;
        delay_ms(1);
        EN = 0;
    }

    delay_us(500);

    LCD_command(0x38);          // 4-bit 5x7
    LCD_command(0x06);          // Increment cursor position; no display shift
    LCD_command(0x0C);          // Display on, cursor off
    LCD_command(0x01);          // Clear display
    LCD_command(0x80);          // Set DDRAM address to 0

    delay_ms(2000);

    LCD_home(LCD_ROW_TOP);

    reset_buffer();
    
    delay_ms(100);
}

void LCD_data(unsigned char data){
    delay_us(50); // Wait for previous data to finish
    RS = 1;
    LCD_write_info(data);
    EN = 1;
    delay_us(50);
    EN = 0;
    delay_us(50);
    // Lower nibble sent
    RS = 0;
}



void LCD_cursor(int on) {
    if (on) {
        LCD_command(0x0F);
    } else {
        LCD_command(0x0C);
    }
}

void LCD_blink(char * msg, lcd_row row, int wait_ms) {
    lcd_row old_row = current_row;
    int old_char = current_char;

    LCD_home(row);
    int index = 0;
    while(index < 16) {
        if (!msg[index]) break;
        LCD_data(msg[index]);
        LCD_command(0x06);
        ++index;
    }
    while(index < 16) {
        LCD_data(' ');
        LCD_command(0x06);
        ++index;
    }
    delay_ms(wait_ms);
    LCD_home(row);
    LCD_print(&(LCD_buffer[row*16]));
    current_row = old_row;
    current_char = old_char;
    LCD_command((1<<7) + current_char + 40*current_row);

}

void LCD_command(unsigned char command){
    delay_us(100); // Wait for previous command to finish
    RS = 0;
    RW = 0;
    delay_us(50);
    LCD_write_info(command);
    EN = 1;
    delay_us(50);
    EN = 0;
    delay_us(50);
    // Lower nibble sent
}



void LCD_char(unsigned char ch){
    if (current_char < 16) {
        LCD_buffer[current_row * 16 + current_char] = ch;
        LCD_data(ch);
        LCD_command(0x06);
        current_char++;
    }
}


void LCD_write_info(unsigned char info) {
    D0 = (info & 0x01);
    D1 = (info & 0x02) >> 1;
    D2 = (info & 0x04) >> 2;
    D3 = (info & 0x08) >> 3;
    D4 = (info & 0x10) >> 4;
    D5 = (info & 0x20) >> 5;
    D6 = (info & 0x40) >> 6;
    D7 = (info & 0x80) >> 7;
}


void LCD_reset(void){	//Clears LCD
    LCD_command(1<<0);
    LCD_home(0);
    delay_ms(10);
    current_row = LCD_ROW_TOP;
    current_char = 0;
    reset_buffer();

}

void LCD_move_cursor(lcd_direction direction) {
    if ((direction == LCD_DIRECTION_RIGHT && current_char == 16) || 
            (direction == LCD_DIRECTION_LEFT && current_char == 0))
        return;
    LCD_command(0x10 | ((0x04)*direction));
    // -1 for left, +1 for right
    current_char += -1 + 2*direction;
}

void LCD_home(lcd_row row) {
    switch(row) {
      case LCD_ROW_TOP:
        LCD_command(0x0002);
        break;
      case LCD_ROW_BOTTOM:
        LCD_command(0x00C0);
        break;
    }
    current_row = row;
    current_char = 0;
}

void LCD_replace_row(char letters[], lcd_row row) {
    LCD_home(row);
    LCD_print("                ");
    LCD_home(row);
    LCD_print(letters);
}

void LCD_print(const char* letters) {
    // set cursor to appropriate position.
    int string_index = 0;

    while (current_char < 16){
        if (letters[string_index]) {
            LCD_char(letters[string_index]);
            string_index++;
        } else {
            break;
        }
    };
}

void LCD_int(uint32_t number) {
    char digits[17];
    int num_digits = 0;

    if (number == 0) {
        LCD_char('0');
    } else {
        while(number) {
            digits[num_digits++] = '0' + number%10;
            number /= 10;
        }
        // reverse
        int i = 0;
        while(i < num_digits/2) {
            int temp = digits[i];
            digits[i] = digits[num_digits-1-i];
            digits[num_digits-1-i] = temp;
            ++i;
        }
        // add string termination
        digits[num_digits++] = 0;
        LCD_print(digits);
    }

}

void LCD_hex(unsigned int number) {
    char digits[16];
    int num_digits = 0;

    if (number == 0) {
        LCD_char('0');
    } else {
        while(number) {
            int digit_num = number%16;
            char digit;
            if (digit_num<10) {
                digit = '0' + digit_num;
            } else {
                digit = 'A' + digit_num-10;
            }
            digits[num_digits++] = digit;
            number /= 16;
        }
        // reverse
        int i = 0;
        while(i < num_digits/2) {
            int temp = digits[i];
            digits[i] = digits[num_digits-1-i];
            digits[num_digits-1-i] = temp;
            ++i;
        }
        // add string termination
        digits[num_digits++] = 0;
        LCD_print(digits);
    }
}

void LCD_float(float f, int digits_after_dot) {
    int int_part = (int)f;
    f -= int_part;
    int digits_after_dot_cpy = digits_after_dot;
    while(digits_after_dot_cpy--) f *= 10.0;
    int float_part = (int)f;
    LCD_int(int_part);
    LCD_char('.');
    int float_digits = 0;
    int float_part_cpy = float_part;
    while(float_part_cpy) { float_digits++; float_part_cpy/=10;}
    int num_zeros = digits_after_dot-float_digits;
    while(num_zeros--) LCD_char('0');
    if (float_part > 0) LCD_int(float_part);
}


//////////////////// CUSTOM CHARACTERS /////////////////////////////////////////

static unsigned char *custom_char_map;

// CUSTOM CHARACTER CACHE
// THERE CAN BE AT MOST 8 DIFFERENT CUSTOM CHARACTERS USED AT THE SAME TIME.
// which character is currently cached in which slot.
static int CCC[] = {-1,-1,-1,-1,-1,-1,-1,-1};
static int CCC_next_index = 0;

void LCD_load_custom_char(unsigned int char_no, int address) {
    // address auto increments, so it only needs to be set once.
    LCD_command((1<<6) | (address<<3)); // set cgram address
    int i=0;
    while (i<8) {
        LCD_data(custom_char_map[char_no * 8 + i]);
        ++i;
    }
}

int CCC_in_cache(int char_no) {
    int i = 0;
    int cache_hit = -1;
    while (i<8) {
        if (CCC[i] == char_no) {
            cache_hit = i;
            break;
        }
        ++i;
    }
    return cache_hit;
}

void LCD_set_custom_char_map(unsigned char *map) {
    custom_char_map = map;
}

void LCD_char_custom(unsigned int char_no) {
    char char_slot = CCC_in_cache(char_no);
    char_slot = -1;
    if (char_slot == -1) {
        CCC_next_index++;
        if (CCC_next_index == 8) CCC_next_index = 0;
        char_slot = CCC_next_index;
        LCD_load_custom_char(char_no, char_slot);
        LCD_command((1<<7) + current_char + 40*current_row);
    }
    // set address to the next char position.
    LCD_char(char_slot);
}

void LCD_print_custom(unsigned int* letters) {
        // set cursor to appropriate position.
    int string_index = 0;

    while (current_char < 16){
        if (letters[string_index]) {
            LCD_char_custom(letters[string_index]);
            string_index++;
        } else {
            break;
        }
    };
}