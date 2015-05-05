#include "enable_model.h"

#include <p33FJ64GS406.h>

#include "shared/utils.h"


double light_port_intensity;
int port1_state;
int port2_state;



int outputs_disabled() {
    return port1_state == 0 &&
           port2_state == 0 &&
           double_abs(light_port_intensity) < 1E-5;
}


void initializePorts(){
    TRISEbits.TRISE7 = 0;    //make 12v enable an output
    TRISGbits.TRISG6 = 0;   //make 5v enable an output

    light_port_intensity = 1000.0;
    port1_state = 1000;
    port2_state = 1000;

    set_light_port_intensity(0.0);
    set_port1(0);
    set_port2(0);
}

void set_light_port_intensity(float percentage) {
    //if (abs(light_port_intensity - percentage) < 1e-10) return;

    double max_value = 0.8 * PTPER;
    PDC4 = (unsigned int)(percentage * max_value);

    light_port_intensity = percentage;
}

void set_port1(int on) {
    // if (port1_state == on) return;

    if (on) LATEbits.LATE7 = 1;
    else LATEbits.LATE7 = 0;

    port1_state = on;
}
void set_port2(int on) {
    // if (port2_state == on) return;

    if (on) LATGbits.LATG6 = 1;
    else LATGbits.LATG6 = 0;

    port2_state = on;
}
