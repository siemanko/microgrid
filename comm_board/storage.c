#include "storage.h"

#include "drivers/eeprom.h"
#include "utils/debug.h"
#include "drivers/timer.h"
#include "user_interface/balance.h"
#include "demand_response/state_of_charge.h"

#define STORAGE_INTEGRITY_CONSTANT 7LL

#define INITIAL_BALANCE 10000

#define FORCE_RESET 0

void init_storage() {
    init_eeprom();
}

void storage_integrity_check() {
    int ok = 1;
    ok = ok && eeprom_read_uint32(STORAGE_INTEGRITY_CHECK) ==
               STORAGE_INTEGRITY_CONSTANT;
    if (!ok) {
        debug(DEBUG_ERROR, "Storage integrity check failed - resetting.");
        storage_factory_reset();
    }
    if (FORCE_RESET) {
        debug(DEBUG_INFO, "Forced storage reset - update the code.");
        storage_factory_reset();
    }
    
}

void storage_load_settings() {
    storage_integrity_check();
    time_set_seconds_since_epoch(eeprom_read_uint32(STORAGE_TIME));
    balance_set(eeprom_read_uint32(STORAGE_BALANCE));
    state_of_charge = eeprom_read_float(STORAGE_STATE_OF_CHARGE);
    uncertertainty_of_charge =
            eeprom_read_float(STORAGE_UNCERTAINTY_OF_CHARGE);
}

void storage_backup() {
    eeprom_write_uint32(STORAGE_TIME, time_seconds_since_epoch());
    eeprom_write_uint32(STORAGE_BALANCE, balance_get());
    eeprom_write_float(STORAGE_STATE_OF_CHARGE, state_of_charge);
    eeprom_write_float(STORAGE_UNCERTAINTY_OF_CHARGE,
            uncertertainty_of_charge);
    
}


void storage_factory_reset() {
    eeprom_wipe();
    eeprom_write_uint32(STORAGE_INTEGRITY_CHECK,
                        STORAGE_INTEGRITY_CONSTANT);
    eeprom_write_byte(STORAGE_UID, 1);
    eeprom_write_byte(STORAGE_NODE_TYPE, 'B');
    eeprom_write_uint32(STORAGE_BALANCE, INITIAL_BALANCE);
    eeprom_write_float(STORAGE_STATE_OF_CHARGE, 1);
    eeprom_write_float(STORAGE_UNCERTAINTY_OF_CHARGE, 1000000.0);
}