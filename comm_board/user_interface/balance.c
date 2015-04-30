#include "balance.h"

static uint32_t balance = 0;

uint32_t balance_get() {
    return balance;
}

void balance_set(uint32_t value) {
    balance = value;
}
void balance_step() {
    if (balance > 0) {
        balance--;
        /*if (LoadBoxShouldUsePowerReadings() && LoadBoxIsPowerConsumed()) {
            // debug(DEBUG_GRID_STATE, "here %d %f",sizeof(float), power);
            uint32_t power_as_int = (int)LoadBoxGetPower();
            if (power_as_int > getBalance()) {
                setBalance(0);
            } else {
                setBalance(getBalance() - power_as_int);
            }
        }*/
    }
}