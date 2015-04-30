#include "communication/interface.h"
#include "drivers/board.h"
#include "drivers/timer.h"
#include "drivers/lcd.h"
#include "drivers/uart.h"
#include "shared/utils.h"
#include "shared/communication/utils/message_builder.h"
#include "storage.h"
#include "utils/debug.h"
#include "utils/misc.h"
#include "utils/cron.h"
#include "drivers/eeprom.h"
#include "user_interface/display.h"
#include "user_interface/balance.h"'

#include "power/load_board_interface.h"


void init(void) {
    init_board();
    init_timer();
    init_assert();
    init_storage();
    init_cron();
    init_communication();
    init_display();
    
    init_load_board_interface();    
    
    storage_load_settings();
        
    debug(DEBUG_INFO, "Initialization sequence complete.");
}

void load_board_test() {
    load_board_ports_on();
    float res;
    if (load_board_output_current(&res)) {
        debug(DEBUG_INFO, "Current is %f", res);
    } else {
        debug(DEBUG_INFO, "Unable to get current reading.");
    }
}

void init_cron_schedule() {    
    cron_repeat_rapidly(communication_step);
    cron_repeat_every_s(10, storage_backup);
    cron_repeat_every_s(1,  display_step);
    cron_repeat_every_s(1,  balance_step);
    
    cron_repeat_every_s(1,  load_board_test);
}

int main() {
    init();
    init_cron_schedule();
    
    while(1) {
        cron_step();
    }

    return 1;
}
   