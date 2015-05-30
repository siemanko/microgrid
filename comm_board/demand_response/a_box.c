#include "a_box.h"

#include "demand_response/common.h"
#include "communication/messages.h"
#include "communication/interface.h"
#include "shared/utils.h"
#include "utils/debug.h"
#include "demand_response/state_of_charge.h"

static int override_active;
static DemandResponeState override_state;
static DemandResponeState current_state;


static void override_demand_reponse_handler(Message* msg) {
    assert(0 <= msg->content[1] && msg->content[1] <= 1);
    assert(0 <= msg->content[2] && msg->content[2] < DR_STATE_TOTAL);
    override_active = msg->content[1];
    override_state = msg->content[2];
    if (override_active) {
        debug(DEBUG_INFO, "Overriding demand response state to %s.",
                dr_state_as_string(override_state));
    } else {
        debug(DEBUG_INFO, "Demand response state override disabled.");
    }
}

void init_a_box_demand_response() {
    current_state = DR_STATE_GREEN;
    override_active = 0;
    set_message_handler(UMSG_OVERRIDE_DEMAND_REPONSE,
            override_demand_reponse_handler);
    init_state_of_charge();
}

DemandResponeState a_box_demand_reponse_current_state() {
    if (override_active) {
        return override_state;
    } else {
        return current_state;
    }
}

static void broadcast_state() {
    MessageBuilder mb;
    make_mb(&mb, 2);
    mb_add_char(&mb, UMSG_DEMAND_REPONSE);
    mb_add_char(&mb, (char)a_box_demand_reponse_current_state());
    send_mb(&mb, BROADCAST_UID);
}


void a_box_demand_response_step() {
    state_of_charge_step();
    broadcast_state();
}