#include "display.h"

#include "drivers/lcd.h"
#include "drivers/leds.h"
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

static int step = 0;


static void update_leds() {
    DemandResponeState state = b_box_demand_reponse_current_state();
    leds_set(LED_TYPE_RED, LED_STATE_OFF);
    leds_set(LED_TYPE_YELLOW, LED_STATE_OFF);
    leds_set(LED_TYPE_GREEN, LED_STATE_OFF);

    if (state == DR_STATE_GREEN) {
        leds_set(LED_TYPE_GREEN, LED_STATE_ON);
    }
    
    if (state == DR_STATE_YELLOW) {
        leds_set(LED_TYPE_YELLOW, LED_STATE_ON);
    }
    
    if (state == DR_STATE_RED) {
        leds_set(LED_TYPE_RED, LED_STATE_ON);
    }
    
    
}

void display_step() {
    update_leds();
    
    LCD_replace_row("State: ", LCD_ROW_TOP);
    LCD_print(dr_state_as_string(b_box_demand_reponse_current_state()));
    if (step++  & 1) LCD_char('*');
    LCD_replace_row("", LCD_ROW_BOTTOM);
    LCD_print_custom(energy_hindi);
    LCD_print(": ");
    LCD_int(balance_get());
}