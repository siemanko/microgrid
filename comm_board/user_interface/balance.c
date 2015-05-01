#include "balance.h"
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
            uint32_t power_as_int = (int)b_box_get_power();
            if (power_as_int > balance_get()) {
                balance_set(0);
            } else {
                balance_set(balance_get() - power_as_int);
            }
        }
    }
}