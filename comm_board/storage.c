#include "storage.h"

#include "constants.h"
#include "utils/debug.h"
#include "drivers/timer.h"
#include "user_interface/balance.h"
#include "demand_response/state_of_charge.h"
#include "demand_response/a_box.h"
#include "user_interface/balance.h"


void init_storage() {
    init_eeprom();
}

void storage_integrity_check() {
    int ok = 1;
    ok = ok && eeprom_read_uint32(STORAGE_INTEGRITY_CHECK) ==
               STORAGE_INTEGRITY_CONSTANT;
    if (!ok) {
        debug_unsafe(DEBUG_ERROR, "Storage integrity check failed - resetting.");
        asm("RESET");
    }
    if (FORCE_FACTORY_RESET) {
        debug_unsafe(DEBUG_INFO, "Forced storage reset - update the code.");
        storage_factory_reset();
        asm("RESET");
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
    
    balance_update_hours = eeprom_read_int(STORAGE_BALANCE_UPDATE_HOURS);
    balance_update_minutes = eeprom_read_int(STORAGE_BALANCE_UPDATE_MINUTES);
    balance_update_ammount = eeprom_read_uint32(STORAGE_BALANCE_UPDATE_AMMOUNT);

}

void storage_backup() {
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
    
    eeprom_write_int(STORAGE_BALANCE_UPDATE_HOURS,
            balance_update_hours);
    eeprom_write_int(STORAGE_BALANCE_UPDATE_MINUTES,
            balance_update_minutes);
    eeprom_write_uint32(STORAGE_BALANCE_UPDATE_AMMOUNT,
            balance_update_ammount);
}


void storage_factory_reset() {
    eeprom_wipe();
    eeprom_write_uint32(STORAGE_INTEGRITY_CHECK,
                        STORAGE_INTEGRITY_CONSTANT);
    eeprom_write_byte(STORAGE_UID, 1);
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

    eeprom_write_int(STORAGE_BALANCE_UPDATE_HOURS,
            DEFAULT_BALANCE_UPDATE_HOURS);
    
    eeprom_write_int(STORAGE_BALANCE_UPDATE_MINUTES,
            DEFAULT_BALANCE_UPDATE_MINUTES);
    
    eeprom_write_uint32(STORAGE_BALANCE_UPDATE_AMMOUNT,
            DEFAULT_BALANCE_UPDATE_AMMOUN);
}