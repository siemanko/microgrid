#include "ComputerMessages.h"


#include "debugging.h"

#include "DateAndTime.h"
#include "settings.h"
#include "LibraryFunctions.h"
#include "lcd.h"
#include "BatteryManagement.h"
#include "SPI_EEPROM.h"
#include "Balance.h"

static void (*COMPUTER_MESSAGE_HANDLERS[MAX_COMPUTER_MESSAGE_HANDLERS]) (char* msg);


void handle_computer_message(char* message) {
    // LCD_replace_line("M: ", LCD_TOP_ROW);
    // LCD_int((int)message[0]);
    if (COMPUTER_MESSAGE_HANDLERS[message[0]] != 0) {
        debug_unsafe(DEBUG_SPAM, "Processing %d", (int)message[0]);
        COMPUTER_MESSAGE_HANDLERS[message[0]](message);
        debug_unsafe(DEBUG_SPAM, "Finished Processing %d", (int)message[0]);

    }
}

void register_computer_message_handler(int num, void (*handler)(char* msg)) {
    COMPUTER_MESSAGE_HANDLERS[num] = handler;
}


void reset_pic(char* ignore) {
     asm("RESET");
}

void handle_ping(char* ignored) {
    debug(DEBUG_MAIN, "ping");
    MessageBuilder mb;
    make_mb(&mb, 2);
    mb_add_char(&mb, 0x1);
    mb_add_char(&mb, 'C');
    send_computer_mb(&mb);
}

void sync_data(char* ignored) {
    MessageBuilder mb;
    // DEBUG MODES
    make_mb(&mb, 2 + SETTINGS_MAX_DEBUG_MODES);
    mb_add_char(&mb, 3);  // SYNC_DATA message
    mb_add_char(&mb, 1);  // DESCRIBING DEBUG MODES
    mb_add_char(&mb, SETTINGS_MAX_DEBUG_MODES);
    int subsystem;
    for (subsystem=1; subsystem <= SETTINGS_MAX_DEBUG_MODES; ++subsystem) {
        mb_add_char(&mb, getSetting(SETTINGs_DEBUG_MODES + subsystem - 1 ));
    }
    send_computer_mb(&mb);
}

void get_value_reponse_int(int val, int i) {
    MessageBuilder mb;
    make_mb(&mb, 4);
    mb_add_char(&mb, GET_VALUE);   // GET_VALUE_REPONSE
    mb_add_char(&mb, val);         // WHICH_VALUE_REPONSE
    mb_add_int_noprefix(&mb, i);            // ACTUAL_VALUE
    send_computer_mb(&mb);
}



void get_value_reponse_float(int val, float f) {
    MessageBuilder mb;
    make_mb(&mb, 6);
    mb_add_char(&mb, GET_VALUE);   // GET_VALUE_REPONSE
    mb_add_char(&mb, val);         // WHICH_VALUE_REPONSE
    mb_add_float_noprefix(&mb, f); // ACTUAL_VALUE
    send_computer_mb(&mb);
}

void get_value(char* message) {
    char val = message[2];

    if (val == VALUE_HOURS) {
        get_value_reponse_int(val, getHours());
    } else if (val == VALUE_MINUTES) {
        get_value_reponse_int(val, getMinutes());
    } else if (val == VALUE_SECONDS) {
        get_value_reponse_int(val, getSeconds());
    } else if (val == VALUE_BATTERY) {
        get_value_reponse_float(val, getBatteryEnergy());
    } else if (val == VALUE_UID) {
        get_value_reponse_int(val, getSetting(SETTINGS_UID));
    } else if (val == VALUE_DAYS) {
        get_value_reponse_int(val, getDays());
    } else if (val == VALUE_DEVICE_TYPE) {
        get_value_reponse_int(val, getSetting(SETTINGS_NODE_TYPE));
    } else if (val == VALUE_BALANCE) {
        get_value_reponse_int(val, getBalance());
    }
}

void set_value(char* message) {
    char val = message[2];
    char* data = &(message[3]);

    if (val == VALUE_HOURS) {
        setHours(BytesToInt(data));
    } else if (val == VALUE_MINUTES) {
        setMinutes(BytesToInt(data));
    } else if (val == VALUE_SECONDS) {
        setSeconds(BytesToInt(data));
    } else if (val == VALUE_BATTERY) {
        setBatteryEnergy(BytesToFloat(data));
    } else if (val == VALUE_UID) {
        setSetting(SETTINGS_UID, BytesToInt(data));
    } else if (val == VALUE_DAYS) {
        setDays(BytesToInt(data));
    } else if (val == VALUE_DEVICE_TYPE) {
        setSetting(SETTINGS_NODE_TYPE, BytesToInt(data));
    } else if (val == VALUE_BALANCE) {
        setBalance(BytesToInt(data));
    }
}


void get_eeprom(char* message) {
    int mode = message[2];
    unsigned short addr = BytesToInt(message + 3);
    if (mode == 0) {  // char
        debug(DEBUG_MAIN, "EEPROM@%d as char is %d", addr, readEEPROMByte(addr));
    } else if (mode == 1) { // int
        debug(DEBUG_MAIN, "EEPROM@%d as int is %d", addr, readEEPROMInt(addr));
    } else if (mode == 2) { // float
        debug(DEBUG_MAIN, "EEPROM@%d as float is %f", addr, readEEPROMFloat(addr));
    }
}

void register_misc_messages() {
    register_computer_message_handler(PING_MESSAGE, handle_ping);
    register_computer_message_handler(RESET_PIC, reset_pic);
    register_computer_message_handler(SYNC_DATA, sync_data);
    register_computer_message_handler(GET_VALUE, get_value);
    register_computer_message_handler(SET_VALUE, set_value);
    register_computer_message_handler(GET_EEPROM, get_eeprom);
}

