#include "Balance.h"

#include "DateAndTime.h"
#include "LoadBoardInterface.h"
#include "debugging.h"
#include "LoadBoxGridManagement.h"
#include "settings.h"
#include "SPI_EEPROM.h"
#include "LibraryFunctions.h"

uint8_t balance_in_cache = 0;
uint32_t balance_cache;
uint32_t last_balance_update = 0;

void initBalance(void) {
}

uint32_t getBalance(void) {
    if (!balance_in_cache) {
        balance_cache = readEEPROMLong(SETTINGS_BALANCE);
        balance_in_cache = 1;
    }
    return balance_cache;
}

void setBalance(uint32_t new_balance) {
   writeEEPROMLong(SETTINGS_BALANCE, new_balance);
   balance_in_cache = 0;
}

void balanceStep(void) {
    if (last_balance_update + 1 <= getTimestamp()) {
        last_balance_update = getTimestamp();
        if (getBalance() > 0) {
            if (LoadBoxShouldUsePowerReadings() && LoadBoxIsPowerConsumed()) {
                // debug(DEBUG_GRID_STATE, "here %d %f",sizeof(float), power);
                uint32_t power_as_int = (int)LoadBoxGetPower();
                if (power_as_int > getBalance()) {
                    setBalance(0);
                } else {
                    setBalance(getBalance() - power_as_int);
                }
            }
        }
    }
}