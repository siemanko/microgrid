#include "settings.h"
#include "timer.h"
#include "debugging.h"

// THIS SHOULD PROBABLY LATER HAVE SOME HASH CACHE.
// TO AVOID MANY WRITES TO CHIP.

unsigned char settings_is_cached[SETTING_LAST_CACHED];
unsigned char settings_cache[SETTING_LAST_CACHED];

unsigned char getSetting(unsigned int addr) {
    if (addr <= SETTING_LAST_CACHED) {
        if (!settings_is_cached[addr]) {
            settings_cache[addr] = readEEPROMByte(addr);
            settings_is_cached[addr] = 1;
        }
        return settings_cache[addr];
    } else {
        return readEEPROMByte(addr);
    }
}

void setSetting(unsigned int addr, unsigned char value) {
    // roughtly 1/2500 s.
    int i;
    for(i=0; i<10000; ++i);
    writeEEPROMByte(addr, value);
    if (addr <= SETTING_LAST_CACHED) {
        settings_is_cached[addr] = 0;
    }
}

int checkMagicNumber(void) {
    return getSetting(SETTINGS_EEPROM_MAGIC) == 42;
}

void resetSettings(unsigned char new_uid, char new_node_type) {
    eraseEEPROM();
    setSetting(SETTINGS_EEPROM_MAGIC, 42);
    setSetting(SETTINGS_UID, new_uid);
    setSetting(SETTINGS_NODE_TYPE, new_node_type);
    int i;
    for(i=0; i<SETTINGS_MAX_DEBUG_MODES; ++i) {
        setSetting(SETTINGs_DEBUG_MODES + i, 0);
    }
    set_debug(DEBUG_MAIN, 1);
    setSetting(SETTING_TIME_DAYS, 0);
    setSetting(SETTING_TIME_HOURS, 0);
    setSetting(SETTING_TIME_MINUTES, 0);

    setBalance(1000);


    for (i=0; i<= SETTING_LAST_CACHED; ++i) {
        settings_is_cached[i] = 0;
    }

}
