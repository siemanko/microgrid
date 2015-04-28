#include "messages.h"
#include "communication/interface.h"
#include "shared/utils.h"
#include "utils/debug.h"

typedef enum {
    MESSAGE_PING = 0
} MessageType;

void handle_message(Message* msg) {
    if (msg->length < 1)
        return;
    
    if (msg->content[0] == MESSAGE_PING) {
        debug(DEBUG_MISC, "pong");
    }
}