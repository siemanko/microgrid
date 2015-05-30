#ifndef COMMUNICATION_MESSAGES_H
#define COMMUNICATION_MESSAGES_H

#include "shared/communication/message.h"

typedef enum {
    UMSG_PING = 0,
    UMSG_GET_SETTINGS = 1,
    UMSG_SET_TIME = 2,
    UMSG_CRON_STATS = 3,
    UMSG_RESET_PIC = 4,
    UMSG_GET_MEMORY = 5,
    UMSG_SET_UID_NODE_TYPE = 6,
    UMSG_SET_BALANCE = 7,
    UMSG_READ_EEPROM = 8,
    UMSG_DATA_LOGGER = 9,
    UMSG_PONG = 10,
    UMSG_GET_CONNECTED_NODES = 11,
    UMSG_TEST_LEDS = 12,
    UMSG_PRINT_LOCAL_TIME = 13,
    UMSG_DEMAND_REPONSE = 14,
    UMSG_OVERRIDE_DEMAND_REPONSE = 15,
    UMSG_PRINT_DATA_LOGS = 16,
    UMSG_SET_STATE_OF_CHARGE = 17,
    // leave last (also make sure no gaps above)
    UMSG_TOTAL_MESSAGES 
} MessageToUlink; 

typedef enum {
    CMSG_DEBUG = 0,
    CMSG_GET_SETTINGS_REPLY = 1,
    CMSG_DATA_LOGGER_REPLY = 2,
    // leave last (also make sure no gaps above)
    CMSG_TOTAL_MESSAGES
} MessageToComputer;

void handle_message(Message*);

void set_message_handler(MessageToUlink msg_type,
                          void (*handler)(Message*));

void register_misc_message_handlers();

#endif