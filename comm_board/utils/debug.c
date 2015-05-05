#include "debug.h"

#include <stdarg.h>
#include <stdlib.h>

#include "communication/interface.h"
#include "communication/messages.h"
#include "shared/communication/utils/message_builder.h"
#include "shared/utils.h"

static void make_debug_mb(MessageBuilder* mb,
                          char subsystem,
                          char* format,
                          va_list args) {
    make_mb(mb, 2);
    mb_add_char(mb, CMSG_DEBUG); // indicate debug message
    mb_add_char(mb, subsystem);
    mb_add_formated_args(mb, format, args);
}

void debug(char subsystem, char* format, ...) {
    va_list args;
    va_start(args, format);
    debug_args(subsystem, format, args);
    va_end(args);
}

void debug_args(char subsystem, char* format, va_list args) {
    MessageBuilder mb;
    make_debug_mb(&mb, subsystem, format, args);
    send((uint8_t*)mb.message, mb.next_char, COMPUTER_UID);
}

void debug_unsafe(char subsystem, char* format, ...) {
    va_list args;
    va_start(args, format);
    debug_unsafe_args(subsystem, format, args);
    va_end(args);
}
void debug_unsafe_args(char subsystem, char* format, va_list args) {
    MessageBuilder mb;
    make_debug_mb(&mb, subsystem, format, args);
    
    Message* msg = (Message*) safe_malloc(sizeof(Message));
    
    msg->source = 1;
    msg->destination = COMPUTER_UID;
    msg->length = (uint8_t)mb.next_char;
    msg->content = (uint8_t*)mb.message;
    computer_send_message_unsafe(msg);
}
