#include <p33FJ64GS406.h>

#include "p33FJ64GS406_drivers/board.h"
#include "p33FJ64GS406_drivers/adc.h"
#include "p33FJ64GS406_drivers/timer.h"
#include "p33FJ64GS406_drivers/spi_slave.h"
#include "drivers/pwm.h"
#include "isr.h"
#include "reading_model.h"
#include "api.h"


int main()
{
    init_board();
    init_timer();
    init_PWM();
    init_ADC(6);

    init_spi_slave();
    spi_slave_set_handle_message(link_board_handle_message);

    TRISFbits.TRISF5 = 0;


    while(1){
        update_readings();
    }
}

