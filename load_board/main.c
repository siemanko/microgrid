#include <p33FJ64GS406.h>

#include "drivers/board.h"
#include "drivers/pwm.h"
#include "drivers/adc.h"
#include "api.h"
#include "enable_model.h"

int main()
{
    set_communication_enable(0);
    init_board();
    init_PWM();
    init_timer();
    init_ADC();

    initializePorts();
    initSPICommBoard();

    long long i;
    for(i=0; i<1000000LL; ++i);
    set_communication_enable(1);


    while(1){}
}
