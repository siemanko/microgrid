#include "communication/interface.h"
#include "communication/other_boards/load_board.h"
#include "communication/other_boards/link_board.h"
#include "drivers/board.h"
#include "drivers/button.h"
#include "drivers/timer.h"
#include "drivers/lcd.h"
#include "drivers/leds.h"
#include "drivers/uart.h"
#include "shared/utils.h"
#include "shared/communication/utils/message_builder.h"
#include "storage.h"
#include "utils/debug.h"
#include "utils/misc.h"
#include "utils/cron.h"
#include "drivers/eeprom.h"
#include "user_interface/display.h"
#include "user_interface/balance.h"
#include "demand_response/b_box.h"
#include "data_logger/a_box.h"
#include "data_logger/b_box.h"
#include "tests.h"
#include "communication/network_utils.h"
#include "demand_response/a_box.h"
#include "demand_response/state_of_charge.h"
#include "drivers/button.h"

void init(void) {
    init_board();
    init_timer();    
    delay_ms(500);
    init_assert();
    init_storage();   
    init_cron();
    init_communication();
    delay_ms(500);
    storage_load_settings();   
 
    if (eeprom_read_byte(STORAGE_NODE_TYPE) == 'A') {
        init_link_board_interface();
        init_a_box_demand_response();
        init_a_box_data_logger();
    } else {
        init_leds();
        init_button();
        init_load_board_interface();
        init_b_box_demand_response();
        init_b_box_data_logger();
        init_display();        
    }   
    
    debug(DEBUG_INFO, "Initialization sequence complete.");
}

void init_cron_schedule() {
    cron_repeat_rapidly(communication_step);
    cron_repeat_every_s(10, storage_backup);
    
    if (eeprom_read_byte(STORAGE_NODE_TYPE) == 'A') {
        cron_repeat_every_s(LOG_DATA_EVERY_S,  a_box_data_logger_step);        
        cron_repeat_every_s(2, discover_nodes);       
        cron_repeat_every_s(1,  a_box_demand_response_step);      
    } else {
        cron_repeat_rapidly(button_step);
        cron_repeat_every_s(1,  display_step);
        cron_repeat_every_s(1,  balance_step);
        cron_repeat_every_s(1,  b_box_demand_response_step);
        cron_repeat_every_s(LOG_DATA_EVERY_S, b_box_data_logger_step);  
    
        //cron_repeat_every_s(1,display_button_status);
    }
}

int main() {
    init();
    init_cron_schedule();
    
    while(1) {
        cron_step();
    }

    return 1;
}
   