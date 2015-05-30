#include <p33FJ64GS406.h>

#include "p33FJ64GS406_drivers/board.h"
#include "p33FJ64GS406_drivers/adc.h"
#include "p33FJ64GS406_drivers/timer.h"
#include "p33FJ64GS406_drivers/spi_slave.h"
#include "drivers/pwm.h"
#include "isr.h"
#include "reading_model.h"
#include "api.h"


// this bit controls wether the pin is input/output
#define LED_TRIS TRISBbits.TRISB8
// this ping actually turn led on/off
#define LED_PIN  LATBbits.LATB8

void blink(uint32_t delay, int num_blinks, int pls_update_readings) {
    uint32_t ctr;
    while(num_blinks--) {
        LED_PIN = 1;
        for(ctr = 0; ctr < delay; ++ctr) {
            if (pls_update_readings)
                update_readings();
        }
        LED_PIN = 0;
        for(ctr = 0; ctr < delay; ++ctr) {
            if (pls_update_readings)
                update_readings();
        }
    }
}


int main()
{
    init_board();
    LED_TRIS = 0;
    
    blink(100000L, 10, 0);

    init_timer(512);
    init_PWM();
    init_ADC(6);

    init_spi_slave();
    spi_slave_set_handle_message(link_board_handle_message);

    TRISFbits.TRISF5 = 0;



    while(1) {
        blink(10000L, 1, 1);
    }
}

