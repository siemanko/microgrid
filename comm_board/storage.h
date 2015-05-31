#ifndef STORAGE_H
#define STORAGE_H

#include "drivers/eeprom.h"

typedef enum {
    STORAGE_INTEGRITY_CHECK = 0,        // uint32
    STORAGE_UID = 4,                    // byte 
    STORAGE_NODE_TYPE = 5,              // byte
    STORAGE_TIME = 6,                   // uint32   
    STORAGE_BALANCE = 10,               // uint32
    STORAGE_STATE_OF_CHARGE = 14,       // float
    STORAGE_UNCERTAINTY_OF_CHARGE = 18, // float
    STORAGE_BATTERY_CAPACITY      = 22, // float
    STORAGE_OFF_THRESHOLD = 26,         // float
    STORAGE_RED_THRESHOLD = 30,         // float
    STORAGE_YELLOW_THRESHOLD = 34,      // float
    STORAGE_BALANCE_UPDATE_HOURS = 38,  // int
    STORAGE_BALANCE_UPDATE_MINUTES = 40,// int
    STORAGE_BALANCE_UPDATE_AMMOUNT = 42,// uint32
            
    STORAGE_DATA_LOGGER = 200,          // all the storage above this address
                                        // is dedicated for data_logger
} StorageAddress;

void init_storage();

// Loads save settings from storage
void storage_load_settings();

// Saves vital information to storage.
void storage_backup();

// Factory resets storage
void storage_factory_reset();

#endif