#include "api.h"

#include "drivers/spi_slave.h"
#include "enable_model.h"
#include "reading_model.h"


#define TURN_LIGHT_ON_MSG                           2
#define TURN_LIGHT_OFF_MSG                          3
#define TURN_PORT1_ON_MSG                           4
#define TURN_PORT1_OFF_MSG                          5
#define TURN_PORT2_ON_MSG                           6
#define TURN_PORT2_OFF_MSG                          7


#define GET_CURRENT_FROM_LOAD_BOARD_MSG             8
#define GET_LINE_VOLTAGE_FROM_LOAD_BOARD_MSG        9
#define GET_PORT1_VOLTAGE_FROM_LOAD_BOARD_MSG       10
#define GET_PORT2_VOLTAGE_FROM_LOAD_BOARD_MSG       11




static int communications_on = 0;

int next_index = 0;
uint8_t buffer[4];


void set_communication_enable(int enabled) {
    communications_on = enabled;
}

void put_float_in_buffer(float f) {
    uint8_t* ptr = (uint8_t*)&f;
    int i;
    for (i=0; i<4; ++i) {
        buffer[i] = *ptr;
        ptr += 1;
    }
    next_index = 0;
}

uint8_t handle_message(uint8_t msg){
    if(communications_on == 0) return 132;
    switch(msg){
        case LOAD_REQUEST_DATA:
            if (next_index < 4) {
                return buffer[next_index++];
            } else {
                // should never happen.
                return 0;
            }
            break;
        case TURN_LIGHT_ON_MSG:
            set_light_port_intensity(0.75);
            return 0;
            break;
        case TURN_LIGHT_OFF_MSG:
            set_light_port_intensity(0.0);
            return 0;
            break;
        case TURN_PORT1_ON_MSG:
            set_port1(1);
            return 0;
            break;
        case TURN_PORT1_OFF_MSG:
            set_port1(0);
            return 0;
            break;
        case TURN_PORT2_ON_MSG:
            set_port2(1);
            return 0;
            break;
        case TURN_PORT2_OFF_MSG:
            set_port2(0);
            return 0;
            break;

        //Following cases are sensor values
        case GET_CURRENT_FROM_LOAD_BOARD_MSG: ;
            put_float_in_buffer(get_output_current());
            return 0;
            //sendPollFloatToComm(9.5);
            break;
        case GET_LINE_VOLTAGE_FROM_LOAD_BOARD_MSG: ;
            put_float_in_buffer(get_network_voltage());
            return 0;
            break;            
        case GET_PORT1_VOLTAGE_FROM_LOAD_BOARD_MSG: ;
            put_float_in_buffer(get_output_voltage());
            return 0;
            break;
        case GET_PORT2_VOLTAGE_FROM_LOAD_BOARD_MSG: ;
            put_float_in_buffer(get_phone_voltage());
            return 0;
            break;
        default:
           break;
    }
    return 0;
}







