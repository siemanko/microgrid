#include "common.h"

#include "constants.h"

const char* dr_state_as_string(DemandResponeState state) {
    if (state == DR_STATE_GREEN) {
        return "green";
    } else if (state == DR_STATE_YELLOW) {
        return "yellow";
    } else if (state == DR_STATE_RED) {
        return "red";
    } else if (state == DR_STATE_OFF) {
        return "off";
    } else if (state == DR_STATE_ON) {
        return "on";
    } else {
        return "unknown state";
    }
}

int state_to_price_coefficient(DemandResponeState state) {
    if (state == DR_STATE_GREEN) {
        return PRICE_COEFF_GREEN;
    } else if (state == DR_STATE_YELLOW) {
        return PRICE_COEFF_YELLOW;
    } else if (state == DR_STATE_RED) {
        return PRICE_COEFF_RED;
    } else if (state == DR_STATE_OFF) {
        return 0;
    } else if (state == DR_STATE_ON) {
        // this is a debug state.
        return 0;
    }
    return -1;
}