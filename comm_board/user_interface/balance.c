#include "balance.h"

#include "constants.h"
#include "demand_response/common.h"
#include "demand_response/b_box.h"
#include "drivers/timer.h"

int balance_update_hours = 1;
int balance_update_minutes = 2;
uint32_t balance_update_ammount = 3;

static int balance_update_happened = 0;


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
    struct tm *lt; 
    lt = ulink_local_time();
    if (lt->tm_hour == balance_update_hours &&
            lt->tm_min == balance_update_minutes) {
        if (!balance_update_happened)
            balance += balance_update_ammount;
        balance_update_happened = 1;
    } else {
        balance_update_happened = 0;
    }
    
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