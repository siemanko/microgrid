#include "drivers/board.h"
#include "drivers/timer.h"
#include "drivers/lcd.h"
#include "drivers/hindi.h"


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
   