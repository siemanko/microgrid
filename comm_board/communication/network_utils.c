#include "network_utils.h"

#include "communication/messages.h"
#include "communication/interface.h"
#include "drivers/timer.h"
#include "utils/debug.h"
#include "shared/algorithm/map.h"

#define TIME_BEFORE_DISCONNECT 20

void ping_handler(Message*);
void pong_handler(Message*);
void get_connected_nodes_handler(Message*);

Map nodes_online;

void init_network_utils() {
    make_map(&nodes_online);

    set_message_handler(UMSG_PING, ping_handler);
    set_message_handler(UMSG_PONG, pong_handler);  
    set_message_handler(UMSG_GET_CONNECTED_NODES, 
            get_connected_nodes_handler);
}

void ping(uint8_t target) {
    MessageBuilder mb;
    make_mb(&mb, 1);
    mb_add_char(&mb, UMSG_PING);
    send_mb(&mb, target);
}

void discover_nodes() {
    while(1) {
        int disconnect_uid = -1;
        int idx;
        for (idx = 0; idx < nodes_online.size; ++idx) {
            uint32_t* last_ping = nodes_online.contents[idx]->value;
            if (*last_ping + TIME_BEFORE_DISCONNECT < 
                    time_seconds_since_epoch()) {
                disconnect_uid = nodes_online.contents[idx]->key;
            }
        }
        if (disconnect_uid == -1) {
            break;
        } else {
            map_remove(&nodes_online, disconnect_uid);
        }
    }
    ping(BOARDCAST_UID);
}




void ping_handler(Message* msg) {
    if (msg->source == COMPUTER_UID) {
        debug(DEBUG_INFO, "pong");
    } else {
        MessageBuilder mb;
        make_mb(&mb, 1);
        mb_add_char(&mb, UMSG_PONG);
        send_mb(&mb, msg->source);
    }
}

void pong_handler(Message* msg) {
    uint32_t* now = (uint32_t*)malloc(sizeof(uint32_t));
    *now = time_seconds_since_epoch();
    map_add(&nodes_online, msg->source, now);
}

void get_connected_nodes_handler(Message* msg) {
    MessageBuilder mb;
    make_mb(&mb, 20);
    mb_add_char(&mb, CMSG_DEBUG);
    mb_add_char(&mb, DEBUG_INFO);
    mb_add_formated(&mb, "%d connected nodes: ", nodes_online.size);
    int idx;
    for (idx=0; idx<nodes_online.size; ++idx) {
        if (idx + 1 == nodes_online.size) {
            mb_add_formated(&mb, "%d", nodes_online.contents[idx]->key);
        } else {
            mb_add_formated(&mb, "%d, ", nodes_online.contents[idx]->key);
        }
    }
    send_mb(&mb, COMPUTER_UID);
}