#include "messages.h"
#include "communication/interface.h"
#include "shared/utils.h"
#include "utils/debug.h"
#include "drivers/timer.h"
#include "shared/communication/utils/message_builder.h"
#include "utils/cron.h"
#include "shared/algorithm/vector.h"
#include "drivers/eeprom.h"
#include "storage.h"
#include "user_interface/balance.h"

void (*message_handler[UMSG_TOTAL_MESSAGES])(Message*);

void handle_message(Message* msg) {
    if (msg->length < 1)
        return;
    
    uint8_t msg_type = msg->content[0];
    if (0 <= msg_type && msg_type < UMSG_TOTAL_MESSAGES &&
            message_handler[msg_type] != 0) {
        message_handler[msg_type](msg);
    }
}

void set_message_handler(MessageToUlink msg_type,
                          void (*handler)(Message*)) {
    message_handler[msg_type] = handler;
}

void ping_handler(Message* msg) {
    debug(DEBUG_INFO, "pong");
}

void get_settings_handler(Message* msg) {
    MessageBuilder mb;
    make_mb(&mb, 11);
    mb_add_char(&mb, CMSG_GET_SETTINGS_REPLY);
    mb_add_uint32_noprefix(&mb, time_seconds_since_epoch());
    mb_add_char(&mb, eeprom_read_byte(STORAGE_UID));
    mb_add_char(&mb, eeprom_read_byte(STORAGE_NODE_TYPE));
    mb_add_uint32_noprefix(&mb, balance_get());

    send_mb(&mb, COMPUTER);
}

void set_time_handler(Message* msg) {
    uint32_t ts = bytes_to_uint32(msg->content + 1);
    time_set_seconds_since_epoch(ts);
    cron_reset_timers();
}

void reset_pic_handler(Message* msg) {
    debug_unsafe(DEBUG_INFO, "Resetting PIC. See you soon!");
    asm("RESET");
}

void get_memory_handler(Message* msg) {
    Vector addresses;
    const int INITIAL_VECTOR_CAPACITY = 500;
    make_vector(&addresses, INITIAL_VECTOR_CAPACITY, 0);
    const int CHUNK_SIZE = 100;
    
    uint32_t mem_used = INITIAL_VECTOR_CAPACITY;
    while(1) {
        void* allocated = malloc(CHUNK_SIZE);
        if (allocated == 0) {
            break;
        } else {
            vector_push_back(&addresses, allocated);
            mem_used += CHUNK_SIZE;
        }
    }
    int vidx;
    for (vidx=0; vidx< addresses.size; ++vidx) {
        free(addresses.buffer[vidx]);
    }
    destroy_vector(&addresses);
    debug(DEBUG_INFO, "Free memory on heap: %l bytes (approximately).",
            mem_used);
}

void set_uid_node_type_handler(Message* msg) {
    assert(msg->length == 3);
    eeprom_write_byte(STORAGE_UID, msg->content[1]);
    eeprom_write_byte(STORAGE_NODE_TYPE, msg->content[2]);
}

void set_balance_handler(Message* msg) {
    assert(msg->length == 5);
    uint32_t balance = bytes_to_uint32(msg->content + 1);
    balance_set(balance);
}

void register_misc_message_handlers() {
    set_message_handler(UMSG_PING, ping_handler);
    set_message_handler(UMSG_GET_SETTINGS, get_settings_handler);
    set_message_handler(UMSG_SET_TIME, set_time_handler);
    set_message_handler(UMSG_RESET_PIC, reset_pic_handler);
    set_message_handler(UMSG_GET_MEMORY, get_memory_handler);
    set_message_handler(UMSG_SET_UID_NODE_TYPE, set_uid_node_type_handler);
    set_message_handler(UMSG_SET_BALANCE, set_balance_handler);
}

