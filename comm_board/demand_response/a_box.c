#include "a_box.h"

#include "demand_response/common.h"
#include "communication/messages.h"
#include "communication/interface.h"

static DemandResponeState current_state;

void init_a_box_demand_response() {
    current_state = DR_STATE_GREEN;
}

static void broadcast_state() {
    MessageBuilder mb;
    make_mb(&mb, 2);
    mb_add_char(&mb, UMSG_DEMAND_REPONSE);
    mb_add_char(&mb, (char)current_state);
    send_mb(&mb, BROADCAST_UID);
}


void a_box_demand_response_step() {
    broadcast_state();
}