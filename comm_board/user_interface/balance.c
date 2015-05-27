#include "balance.h"

#include "constants.h"
#include "demand_response/common.h"
#include "demand_response/b_box.h"


static uint32_t balance = 0;

uint32_t balance_get() {
    return balance;
}

void balance_set(uint32_t value) {
    balance = value;
}

void balance_step() {
    if (balance > 0) {
        if (b_box_readings_ready()) {
            // debug(DEBUG_GRID_STATE, "here %d %f",sizeof(float), power);
            uint32_t price = (int)b_box_get_power();
            price *= state_to_price_coefficient(
                    b_box_demand_reponse_current_state());
            if (price > balance_get()) {
                balance_set(0);
            } else {
                balance_set(balance_get() - price);
            }
        }
    }
}