#include "drivers/board.h"
#include "drivers/timer.h"
#include "drivers/lcd.h"
#include "drivers/hindi.h"
#include "drivers/uart.h"

#include "communication/interface.h"
#include "shared/utils.h"

#include "shared/communication/utils/message_builder.h"

void init(void) {
    init_board();
   
    init_timer();
        
    init_communication();
    
    init_uart(UART_DEVICE1);
    
    init_LCD();
}

int main() {
    init();
    
    LCD_set_custom_char_map(hindi_chars);
        
    while(1) {
        LCD_reset();
        MessageBuilder mb;
        make_mb(&mb, 11);
        mb_add_formated(&mb, "pozdro%f!!! (yeah: %d)", 3.14, 111);
        send((uint8_t*)mb.message, mb.next_char, COMPUTER);
        
        LCD_replace_row("hello", LCD_ROW_TOP);
        delay_ms(1000);
        LCD_reset();
        LCD_print_custom(energy_hindi);
        delay_ms(1000);
        uart_put(UART_DEVICE1, 77);

        communication_step();
    }
    return 1;
}
   