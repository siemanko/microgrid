#include "debug.h"

#include <stdarg.h>

#include "communication/interface.h"
#include "communication/messages.h"
#include "shared/communication/utils/message_builder.h"



void debug(char subsystem, char* format, ...) {
    MessageBuilder mb;
    make_mb(&mb, 2);
    mb_add_char(&mb, CMSG_DEBUG); // indicate debug message
    mb_add_char(&mb, subsystem);
    va_list args;
    va_start(args, format);
    mb_add_formated_args(&mb, format, args);
    va_end(args);
    send((uint8_t*)mb.message, mb.next_char, COMPUTER);
}