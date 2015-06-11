#include "storage.h"
#include "constants.h"
#include "utils/debug.h"
#include "drivers/timer.h"
#include "user_interface/balance.h"
#include "demand_response/state_of_charge.h"
#include "demand_response/a_box.h"
#include "shared/utils.h"


void init_storage() {
    init_eeprom();
}

void storage_integrity_check() {
    int ok = 1;
    ok = ok && eeprom_read_uint32(STORAGE_INTEGRITY_CHECK) ==
               STORAGE_INTEGRITY_CONSTANT;
    if (!ok) {
        debug(DEBUG_ERROR, "Storage integrity check failed - resetting.");
        //storage_factory_reset();  //DS:  Edit, should be inlcuded
        reboot_chip();          //DS:  Edit, should be removed
    }
    if (FORCE_FACTORY_RESET) {
        debug(DEBUG_INFO, "Forced storage reset - update the code.");
        storage_factory_reset();
    }
    
}

void storage_load_settings() {
    storage_integrity_check();
    time_set_seconds_since_epoch(eeprom_read_uint32(STORAGE_TIME));
    balance_set(eeprom_read_uint32(STORAGE_BALANCE));    
    
    state_of_charge_q = eeprom_read_float(STORAGE_STATE_OF_CHARGE);
    uncertertainty_of_charge =
            eeprom_read_float(STORAGE_UNCERTAINTY_OF_CHARGE);
    battery_capacity_q =
            eeprom_read_float(STORAGE_BATTERY_CAPACITY);
    
    
    off_threshold = eeprom_read_float(STORAGE_OFF_THRESHOLD);
    red_threshold = eeprom_read_float(STORAGE_RED_THRESHOLD);
    yellow_threshold = eeprom_read_float(STORAGE_YELLOW_THRESHOLD);
}

void storage_backup() {
    
    debug(DEBUG_INFO, "made storage backup");
    
    eeprom_write_uint32(STORAGE_TIME, time_seconds_since_epoch());
    eeprom_write_uint32(STORAGE_BALANCE, balance_get());
    eeprom_write_float(STORAGE_STATE_OF_CHARGE, state_of_charge_q);
    eeprom_write_float(STORAGE_UNCERTAINTY_OF_CHARGE,
            uncertertainty_of_charge);
    eeprom_write_float(STORAGE_BATTERY_CAPACITY,
            battery_capacity_q);
    
    eeprom_write_float(STORAGE_OFF_THRESHOLD,
            off_threshold);
    eeprom_write_float(STORAGE_RED_THRESHOLD,
            red_threshold);    
    eeprom_write_float(STORAGE_YELLOW_THRESHOLD,
            yellow_threshold);    
}


void storage_factory_reset() {
    eeprom_wipe();
    eeprom_write_uint32(STORAGE_INTEGRITY_CHECK,
                        STORAGE_INTEGRITY_CONSTANT);
    eeprom_write_byte(STORAGE_UID, 12);
    eeprom_write_byte(STORAGE_NODE_TYPE, 'B');
    eeprom_write_uint32(STORAGE_BALANCE, INITIAL_BALANCE);
    // state of charge
    eeprom_write_float(STORAGE_STATE_OF_CHARGE, 1);
    eeprom_write_float(STORAGE_UNCERTAINTY_OF_CHARGE,
            DEFAULT_BATTERY_CAPACITY_Q);
    eeprom_write_float(STORAGE_BATTERY_CAPACITY,
            DEFAULT_BATTERY_CAPACITY_Q);
    // thresholds
    eeprom_write_float(STORAGE_OFF_THRESHOLD,
            DEFAULT_OFF_THRESHOLD);
    eeprom_write_float(STORAGE_RED_THRESHOLD,
            DEFAULT_RED_THRESHOLD);
    eeprom_write_float(STORAGE_YELLOW_THRESHOLD,
            DEFAULT_YELLOW_THRESHOLD);
  
}