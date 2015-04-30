#include "storage.h"

#include "drivers/eeprom.h"
#include "utils/debug.h"
#include "drivers/timer.h"
#include "user_interface/balance.h"

#define STORAGE_INTEGRITY_CONSTANT 1LL

#define INITIAL_BALANCE 1000

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
    
}

void storage_load_settings() {
    storage_integrity_check();
    time_set_seconds_since_epoch(eeprom_read_uint32(STORAGE_TIME));
    balance_set(eeprom_read_uint32(STORAGE_BALANCE));
}

void storage_backup() {
    eeprom_write_uint32(STORAGE_TIME, time_seconds_since_epoch());
    eeprom_write_uint32(STORAGE_BALANCE, balance_get());

}


void storage_factory_reset() {
    eeprom_wipe();
    eeprom_write_uint32(STORAGE_INTEGRITY_CHECK,
                        STORAGE_INTEGRITY_CONSTANT);
    eeprom_write_byte(STORAGE_UID, 1);
    eeprom_write_byte(STORAGE_NODE_TYPE, 'B');
    eeprom_write_uint32(STORAGE_BALANCE, INITIAL_BALANCE);
}