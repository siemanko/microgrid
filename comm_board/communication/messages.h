#ifndef COMMUNICATION_MESSAGES_H
#define COMMUNICATION_MESSAGES_H

#include "shared/communication/message.h"

typedef enum {
    UMSG_PING = 0
} MessageToUlink;

typedef enum {
    CMSG_DEBUG = 0
} MessageToComputer;

void handle_message(Message*);

#endif