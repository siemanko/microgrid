#include "b_box.h"

#include "communication/other_boards/load_board.h"
#include "demand_response/common.h"
#include "user_interface/balance.h"
#include "utils/debug.h"
#include "communication/messages.h"
#include "shared/utils.h"
#include "drivers/timer.h"

#define BAD_READINGS_BEFORE_SWITCHOFF 5
#define MAX_TIME_BETWEEN_BROADCASTS_S 5

uint32_t last_state_broadcast = 0;
static DemandResponeState current_state;

int should_results_be_used = 0;

int num_bad_readings_in_row = 0;

// current at the output of load converter
// the current reading does include all loads but does not include
// current consumed by boards.
float output_current;
// voltage at the input to power converter, should be more or less the same
// for all boards.
float network_voltage;
// this is voltage at the output of load converter. It is also the voltage
// used at port1.
float output_voltage;
// voltage at port2
float phone_voltage;

uint32_t grid_management_last_update;

int b_box_is_power_consumed() {
    return output_current > 0.02;
}

float b_box_get_power() {
    return output_current * output_voltage;
}

static void demand_reponse_handler(Message* msg) {
    assert(0 <= msg->content[1] && msg->content[2] < DR_STATE_TOTAL);
    current_state = msg->content[1];
    last_state_broadcast = time_seconds_since_epoch();
}

const char* dr_state_as_string() {
    if (current_state == DR_STATE_GREEN) {
        return "green";
    } else if (current_state == DR_STATE_YELLOW) {
        return "yellow";
    } else if (current_state == DR_STATE_RED) {
        return "red";
    } else if (current_state == DR_STATE_OFF) {
        return "off";
    } else {
        assert(0);
        return "ERROR";
    }
}

void init_b_box_demand_response() {    
    grid_management_last_update = 0;
    should_results_be_used = 0;
    num_bad_readings_in_row = 0;
    current_state = DR_STATE_OFF;
    set_message_handler(UMSG_DEMAND_REPONSE, demand_reponse_handler);
}

// Basic sanity checks on readings be get from load board
int validate_readings() {
    int output_current_normal = 0.0 <= output_current && output_current <= 1000.0;
    int network_voltage_normal =  0.1 <= network_voltage && network_voltage <= 60.0;
    int output_voltage_normal = 0.0 <= output_voltage && output_voltage <= 20.0;
    int phone_voltage_normal = 0.0 <= phone_voltage && phone_voltage <= 10.0;
    return output_current_normal &&
           network_voltage_normal &&
           output_voltage_normal &&
           phone_voltage_normal;
}

// True if the readings currently in buffer are valid.
int b_box_readings_ready() {
    return should_results_be_used;
}

// Tries to pull readings from load board.
void update_readings() {
    should_results_be_used =
            load_board_output_current(&output_current) &&
            load_board_network_voltage(&network_voltage);
            load_board_output_voltage(&output_voltage);
            load_board_phone_voltage(&phone_voltage);
    should_results_be_used = should_results_be_used &&
            validate_readings();
    if (!should_results_be_used) {
        // debug(DEBUG_INFO, "Problem getting readings from load board");
        ++num_bad_readings_in_row;
    } else {
        num_bad_readings_in_row = 0;
    }
}

void b_box_demand_response_step() {
    if (last_state_broadcast + MAX_TIME_BETWEEN_BROADCASTS_S < 
            time_seconds_since_epoch()) {
        current_state = DR_STATE_OFF;
    }
    update_readings();

    if (current_state == DR_STATE_OFF) {
        load_board_ports_off();
        return;
    }
    
    if (b_box_readings_ready()) {
        if (balance_get() == 0 && b_box_is_power_consumed()) {
            load_board_ports_off();
            // return;
        }
        if (balance_get() > 0 && !b_box_is_power_consumed()) {
            load_board_ports_on();
        }
    } else {
        if (num_bad_readings_in_row > BAD_READINGS_BEFORE_SWITCHOFF)
            load_board_ports_off();
    }

}

