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
    // leave last (also make sure no gaps above)
    UMSG_TOTAL_MESSAGES 
} MessageToUlink; 

typedef enum {
    CMSG_DEBUG = 0,
    CMSG_GET_SETTINGS_REPLY = 1,
    // leave last (also make sure no gaps above)
    CMSG_TOTAL_MESSAGES
} MessageToComputer;

void handle_message(Message*);

void set_message_handler(MessageToUlink msg_type,
                          void (*handler)(Message*));

void register_misc_message_handlers();

#endif