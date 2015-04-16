#ifndef SETTINGS_H
#define	SETTINGS_H

#include "SPI_EEPROM.h"

#define SETTINGS_UID 1          // Unique (hopefully) id of this box
#define SETTINGS_NODE_TYPE 2    // type of box (can be 'A' or 'B').
#define SETTINGs_DEBUG_MODES 3  // Setting for various debug modes
                                // Should proably reserve at least
                                // 30 spots for those. See debugging.h
                                // it's 1 if debug mode is enabled.
#define SETTINGS_MAX_DEBUG_MODES 30
#define SETTING_THAT_WILL_BE_NEXT 33 // read comment above
#define SETTING_TIME_MINUTES 34
#define SETTING_TIME_HOURS 35
#define SETTING_TIME_DAYS 36
#define SETTINGS_EEPROM_MAGIC 37 // this number should always be 42,
                                // otherwise we assume EEPROM was not
                                // initialized properly.
#define SETTINGS_BALANCE 42     // takes 4 bytes

#define SETTING_LAST_CACHED 37

// POSSIBLY ADD CACHING IN THE FUTURE
unsigned char getSetting(unsigned int addr);

void setSetting(unsigned int addr, unsigned char value);

int checkMagicNumber(void);

void resetSettings(unsigned char new_uid, char new_node_type);






#endif	/* SETTINGS_H */

