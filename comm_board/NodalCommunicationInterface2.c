#include "NodalCommunicationInterface2.h"
#include "debugging.h"
#include "stdint.h"

#include <stdlib.h>

void process_network_message(NetworkMessage* msg) {
    debug(DEBUG_NETWORK, "message from %d to %d of type %d", msg->source,
                                                             msg->target,
                                                             msg->type);
    debug(DEBUG_NETWORK, msg->body);
}

