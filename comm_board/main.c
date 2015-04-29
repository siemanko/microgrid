#include "communication/interface.h"
#include "drivers/board.h"
#include "drivers/timer.h"
#include "drivers/lcd.h"
#include "drivers/hindi.h"
#include "drivers/uart.h"
#include "shared/utils.h"
#include "shared/communication/utils/message_builder.h"
#include "utils/debug.h"
#include "utils/misc.h"
#include "utils/cron.h"

void init(void) {
    init_board();
   
    init_timer();
    
    init_assert();
    
    init_cron();
        
    init_communication();
        
    init_LCD();
    
    LCD_set_custom_char_map(hindi_chars);
}

void say_hi() {
    debug(DEBUG_INFO, "hello %f!!! (yeah: %d)", 3.14, sizeof(void (*)()));
}

int round = 0;

void ui_showcase() {
    if (round % 2) {
        LCD_replace_row("hello", LCD_ROW_TOP);
    } else {
        LCD_reset();
        LCD_print_custom(energy_hindi);
    }
    ++round;
}

void init_cron_schedule() {
    cron_repeat_rapidly(communication_step);
    cron_repeat_every_s(1, say_hi);
    cron_repeat_every_s(1, ui_showcase);
}

int main() {
    init();
    init_cron_schedule();
        
    while(1) {
        cron_step();
    }

    return 1;
}
   