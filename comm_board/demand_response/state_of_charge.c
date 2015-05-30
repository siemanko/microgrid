#include "state_of_charge.h"

#include "communication/messages.h"
#include "shared/utils.h"


// battery_capacity
float state_of_charge          = 1.0;
float uncertertainty_of_charge = 10000.0;

static void set_state_of_charge_handler(Message* msg) {
    assert(msg->length == 9);
    state_of_charge = bytes_to_float(msg->content + 1);
    uncertertainty_of_charge = bytes_to_float(msg->content + 5);
}


void init_state_of_charge() {
    set_message_handler(UMSG_SET_STATE_OF_CHARGE,
            set_state_of_charge_handler);

}

void state_of_charge_step() {
    
}