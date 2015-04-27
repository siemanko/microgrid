#include "drivers/board.h"
#include "drivers/timer.h"
#include "drivers/lcd.h"
#include "drivers/hindi.h"
#include "drivers/uart.h"

#include "communication/interface.h"
#include "shared/utils.h"


// Internal FRC Oscillator
_FOSCSEL(FNOSC_FRCPLL);  // FRC Oscillator
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_NONE);   // Clock Switching is enabled and Fail Safe Clock Monitor is disabled
						     // OSC2 Pin Function: OSC2 is Clock Output
                                                     // Primary Oscillator Mode: Disabled
_FWDT(FWDTEN_OFF);              // Watchdog Timer Enabled/disabled by user software
_FGS(GWRP_OFF); // General segment Write protect turned off
_FICD(ICS_PGD2 & JTAGEN_OFF) // Select Debug/Program Lines and JTAG OFF


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
        LCD_replace_row("hello", LCD_ROW_TOP);
        send_string((uint8_t*)copy_string("hello"), COMPUTER);
        delay_ms(1000);
        LCD_reset();
        LCD_print_custom(energy_hindi);
        delay_ms(1000);
        uart_put(UART_DEVICE1, 77);

        communication_step();
    }
    return 1;
}
   