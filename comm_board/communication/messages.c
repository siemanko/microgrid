#include "messages.h"
#include "communication/interface.h"
#include "shared/utils.h"
#include "utils/debug.h"
#include "drivers/timer.h"
#include "shared/communication/utils/message_builder.h"
#include "utils/cron.h"

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

void get_time_handler(Message* msg) {
    MessageBuilder mb;
    make_mb(&mb, 5);
    mb_add_char(&mb, CMSG_GET_TIME_REPLY);
    mb_add_uint32_noprefix(&mb, time_seconds_since_epoch());
    send_mb(&mb, COMPUTER);
}

void set_time_handler(Message* msg) {
    uint32_t ts = bytes_to_uint32(msg->content + 1);
    time_set_seconds_since_epoch(ts);
    cron_reset_timers();
}

void reset_pic_handler(Message* msg) {
    asm("RESET");
}

void register_misc_message_handlers() {
    set_message_handler(UMSG_PING, ping_handler);
    set_message_handler(UMSG_GET_TIME, get_time_handler);
    set_message_handler(UMSG_SET_TIME, set_time_handler);
    set_message_handler(UMSG_RESET_PIC, reset_pic_handler);
}

