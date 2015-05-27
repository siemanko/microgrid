#include "display.h"

#include "drivers/lcd.h"
#include "demand_response/b_box.h"
#include "user_interface/hindi.h"
#include "user_interface/balance.h"
#include "drivers/eeprom.h"
#include "storage.h"

void init_display() {
    init_LCD();
    LCD_set_custom_char_map(hindi_chars);

    LCD_replace_row("UID: ?", LCD_ROW_TOP);
    LCD_print(" type: ?");
    LCD_replace_row("", LCD_ROW_BOTTOM); 
    LCD_print_custom(energy_hindi);
    LCD_print(": ?");
}

int step = 0;

void display_step() {
    LCD_replace_row("State: ", LCD_ROW_TOP);
    LCD_print(dr_state_as_string());
    if (step++  & 1) LCD_char('*');
    LCD_replace_row("", LCD_ROW_BOTTOM);
    LCD_print_custom(energy_hindi);
    LCD_print(": ");
    LCD_int(balance_get());
}