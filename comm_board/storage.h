#ifndef STORAGE_H
#define STORAGE_H

typedef enum {
    STORAGE_INTEGRITY_CHECK = 0,       // uint32
    STORAGE_UID = 4,                   // byte 
    STORAGE_NODE_TYPE = 5,             // byte
    STORAGE_TIME = 6,                  // uint32   
    STORAGE_BALANCE = 10,              // uint32
    STORAGE_STATE_OF_CHARGE = 14,       // float
    STORAGE_UNCERTAINTY_OF_CHARGE = 18, // float
    STORAGE_DATA_LOGGER = 200,         // all tge storage above this address
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