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
#include "drivers/leds.h"

void (*message_handler[UMSG_TOTAL_MESSAGES])(Message*);

void handle_message(Message* msg) {
    // TODO(szymon): cache UID..
    if (msg->destination != my_uid &&
            msg->destination != BOARDCAST_UID)
        return;
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

void get_settings_handler(Message* msg) {
    MessageBuilder mb;
    make_mb(&mb, 11);
    mb_add_char(&mb, CMSG_GET_SETTINGS_REPLY);
    mb_add_uint32_noprefix(&mb, time_seconds_since_epoch());
    mb_add_char(&mb, eeprom_read_byte(STORAGE_UID));
    mb_add_char(&mb, eeprom_read_byte(STORAGE_NODE_TYPE));
    mb_add_uint32_noprefix(&mb, balance_get());

    send_mb(&mb, COMPUTER_UID);
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

void read_eeprom_handler(Message* msg) {
    assert(msg->length == 6);
    int variable_type = msg->content[1];
    uint32_t addr = bytes_to_uint32(msg->content + 2);
    if (variable_type == 0) { // float
        float f_read = eeprom_read_float((uint16_t)addr);
        debug(DEBUG_INFO, "EEPROM address %l contains float %f.",
                addr, f_read); 
    } else if (variable_type == 1) { // uint32
        uint32_t l_read = eeprom_read_uint32((uint16_t)addr);
        debug(DEBUG_INFO, "EEPROM address %l contains uint32 %l.",
                addr, l_read);       
    } else if (variable_type == 2) { // byte
        uint32_t b_read = eeprom_read_byte((uint16_t)addr);
        debug(DEBUG_INFO, "EEPROM address %l contains byte %d.",
                addr, (int)b_read); 
    }
}

void test_leds_handler(Message* msg) {
    leds_blink(5);
}

void print_local_time_handler(Message* msg) {
    struct tm *lt; 
    lt = ulink_local_time();
    debug(DEBUG_INFO, "Local time: %s", asctime(lt));
}

void register_misc_message_handlers() {
    set_message_handler(UMSG_GET_SETTINGS, get_settings_handler);
    set_message_handler(UMSG_SET_TIME, set_time_handler);
    set_message_handler(UMSG_RESET_PIC, reset_pic_handler);
    set_message_handler(UMSG_GET_MEMORY, get_memory_handler);
    set_message_handler(UMSG_SET_UID_NODE_TYPE, set_uid_node_type_handler);
    set_message_handler(UMSG_SET_BALANCE, set_balance_handler);
    set_message_handler(UMSG_READ_EEPROM, read_eeprom_handler);
    set_message_handler(UMSG_TEST_LEDS, test_leds_handler);
    set_message_handler(UMSG_PRINT_LOCAL_TIME, print_local_time_handler);
}

