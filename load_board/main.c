#include <p33FJ64GS406.h>

#include "p33FJ64GS406_drivers/board.h"
#include "drivers/pwm.h"
#include "p33FJ64GS406_drivers/adc.h"
#include "p33FJ64GS406_drivers/timer.h"
#include "p33FJ64GS406_drivers/spi_slave.h"
#include "api.h"
#include "enable_model.h"

// Causes all ports to be enabled by default when 
// load board boots.
// This wariable should only be set to 1 for the purpose
// of testing loadboard. For production system it should
// be set to 0.
#define ENABLED_BY_DEFAULT 0

#define LED_TRIS TRISCbits.TRISC14
#define LED_PIN  LATCbits.LATC14

void blink(uint32_t delay, int num_blinks) {
    uint32_t ctr;
    while(num_blinks--) {
        LED_PIN = 1;
        for(ctr = 0; ctr < delay; ++ctr);
        LED_PIN = 0;
        for(ctr = 0; ctr < delay; ++ctr);
    }
}

int main()
{
    // set led pin as output
    LED_TRIS = 0;
    
    blink(30000L, 10);
    
    set_communication_enable(0);
    init_board();
    init_PWM();
    init_timer(20*512);
    init_ADC(12);

    initializePorts();
    init_spi_slave();
    spi_slave_set_handle_message(load_board_handle_message);

    long long i;
    for(i=0; i<1000000LL; ++i);
    set_communication_enable(1);

    if (ENABLED_BY_DEFAULT) {
        set_light_port_intensity(0.75);
        set_port1(1);
        set_port2(1);
    }
    while(1){
        blink(1000000L, 1);
    }
}
