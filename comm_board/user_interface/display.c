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

static uint32_t divider[]    = { 86400, 3600, 60,  1 };
static char divider_letter[] = { 'd',   'h',  'm', 's' };


void print_time(uint32_t td) {
    int didx;
    for (didx = 0; didx < 4; ++didx) {
        if (td >= divider[didx]) {
            LCD_int(td / divider[didx]);
            LCD_char(divider_letter[didx]);
            td %= divider[didx];
        }
    }
}

void display_step() {
    update_leds();
    if (waiting_for_confirmation()) {
        LCD_replace_row("Price went up :(",   LCD_ROW_TOP);
        LCD_replace_row("Push to continue", LCD_ROW_BOTTOM);
    } else {
        LCD_replace_row("", LCD_ROW_TOP);
        int32_t etr = balance_estimated_time_remaining();
        if (etr == -1) {
            LCD_print("Appliances off.");
        } else {
            LCD_char_custom(6);
            LCD_char(' ');
            print_time(etr);
        }
        LCD_replace_row("", LCD_ROW_BOTTOM);
        LCD_print_custom(energy_hindi);
        LCD_print(": ");
        LCD_int(balance_get());
        if (step++  & 1) LCD_char('*');

    }
}