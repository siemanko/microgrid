#include "communication/interface.h"
#include "drivers/board.h"
#include "drivers/timer.h"
#include "drivers/lcd.h"
#include "drivers/hindi.h"
#include "drivers/uart.h"
#include "shared/utils.h"
#include "shared/communication/utils/message_builder.h"
#include "storage.h"
#include "utils/debug.h"
#include "utils/misc.h"
#include "utils/cron.h"
#include "drivers/eeprom.h"

extern Ethermini computer_network;

void init(void) {
    init_board();
    init_timer();
    init_assert();
    init_storage();
    init_cron();
    init_communication();
    init_LCD();
    
    storage_load_settings();
    
    LCD_set_custom_char_map(hindi_chars);
    
    debug(DEBUG_INFO, "Initialization sequence complete.");
}


void ui_showcase() {
    LCD_replace_row("ST: ", LCD_ROW_TOP);
    LCD_int(computer_network.state);
    LCD_replace_row("SA: ", LCD_ROW_BOTTOM);
    LCD_int(computer_network.state_aux);
}

void init_cron_schedule() {
    cron_repeat_rapidly(communication_step);
    cron_repeat_every_s(10, storage_backup);
    cron_repeat_every_ms(500, ui_showcase);

}

int main() {
    init();
    init_cron_schedule();
    
    
    while(1) {
        cron_step();
    }

    return 1;
}
   