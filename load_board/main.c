#include <p33FJ64GS406.h>

#include "shared/p33FJ64GS406_drivers/board.h"
#include "drivers/pwm.h"
#include "shared/p33FJ64GS406_drivers/adc.h"
#include "shared/p33FJ64GS406_drivers/timer.h"
#include "shared/p33FJ64GS406_drivers/spi_slave.h"
#include "api.h"
#include "enable_model.h"

int main()
{
    set_communication_enable(0);
    init_board();
    init_PWM();
    init_timer();
    init_ADC(12);

    initializePorts();
    init_spi_slave();
    spi_slave_set_handle_message(load_board_handle_message);

    long long i;
    for(i=0; i<1000000LL; ++i);
    set_communication_enable(1);


    while(1){}
}
