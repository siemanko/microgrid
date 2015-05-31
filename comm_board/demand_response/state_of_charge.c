#include "state_of_charge.h"

#include "constants.h"
#include "communication/messages.h"
#include "shared/utils.h"
#include "storage.h"
#include "data_logger/a_box.h"


// battery_capacity
float state_of_charge_q         = 1.0;
float uncertertainty_of_charge  = DEFAULT_BATTERY_CAPACITY_Q;
float battery_capacity_q        = DEFAULT_BATTERY_CAPACITY_Q;

static void set_state_of_charge_handler(Message* msg) {
    assert(msg->length == 9);
    state_of_charge_q = bytes_to_float(msg->content + 1);
    uncertertainty_of_charge = bytes_to_float(msg->content + 5);
}

static void set_battery_capacity_handler(Message* msg) {
    assert(msg->length == 5);
    battery_capacity_q = bytes_to_float(msg->content + 1);
    eeprom_write_float(STORAGE_BATTERY_CAPACITY, battery_capacity_q);
}


void init_state_of_charge() {
    set_message_handler(UMSG_SET_STATE_OF_CHARGE,
            set_state_of_charge_handler);
    set_message_handler(UMSG_SET_BATTERY_CAPACITY,
            set_battery_capacity_handler);

}

float get_state_of_charge_percentage() {
    // TODO: consider using uncertainty as well.
    return state_of_charge_q / battery_capacity_q;
}

void state_of_charge_step() {
    int success;
    float battery_input_current;
    float battery_output_current;
    float ignored1;
    float ignored2;
    
    a_box_pull_data(&success,
                    &battery_input_current,
                    &battery_output_current,
                    &ignored1,
                    &ignored2);
    if (!success) return;
    state_of_charge_q += battery_input_current - battery_output_current;
    uncertertainty_of_charge +=
            CURRENT_SENSOR_UNCERTAINTY * (battery_input_current
                + battery_output_current);
}