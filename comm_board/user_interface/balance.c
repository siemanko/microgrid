#include "balance.h"

#include "constants.h"
#include "demand_response/common.h"
#include "demand_response/b_box.h"


static uint32_t balance = 0;
static uint32_t last_price = 0;
uint32_t balance_get() {
    return balance;
}

void balance_set(uint32_t value) {
    balance = value;
}

int32_t balance_estimated_time_remaining() {
    if (last_price == 0) {
        return -1;
    } else {
        return balance / last_price;
    }
}


void balance_step() {
    if (balance > 0) {
        if (b_box_readings_ready()) {
            // debug(DEBUG_GRID_STATE, "here %d %f",sizeof(float), power);
            uint32_t price = (int)b_box_get_power();
            price *= state_to_price_coefficient(
                    b_box_demand_reponse_current_state());
            last_price = price;
            if (price > balance_get()) {
                balance_set(0);
            } else {
                balance_set(balance_get() - price);
            }
        }
    }
}