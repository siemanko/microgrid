#include "debugging.h"


void set_debug_handler(char* msg) {
    int subsystem = msg[2];
    int should_work = msg[3];
    set_debug(subsystem, should_work);
}

void init_debug(void) {
    register_computer_message_handler(SET_DEBUG_MODE, set_debug_handler);
}

void debug(unsigned char subsystem, const char* message, ...) {
    if (should_print_debug(subsystem)) {
        int num_ints = 0;
        int num_floats = 0;
        int str_length = strlen(message);
        int i;
        for(i=1; i < str_length; ++i) {
            if (message[i-1] == '%' && message[i] == 'd') ++num_ints;
            if (message[i-1] == '%' && message[i] == 'f') ++num_floats;
        }
        va_list args;
        va_start(args, num_ints + num_floats);
        MessageBuilder mb;
        make_debug_mb(&mb, subsystem, str_length + 4*num_ints + 6 * num_floats);
        for(i=0; i < str_length; ++i) {
            if (message[i] == '%' && message[i+1] == 'd') {
                mb_add_int(&mb, va_arg(args, int));
                ++i;
            } else if (message[i] == '%' && message[i+1] == 'f') {
                mb_add_float(&mb, va_arg(args, float));
                ++i;
            } else {
                mb_add_char(&mb, message[i]);
            }
        }
        va_end(args);
        send_computer_mb(&mb);
    }
}

void debug_unsafe(unsigned char subsystem, const char* message, ...) {
    if (should_print_debug(subsystem)) {
        int num_ints = 0;
        int num_floats = 0;
        int str_length = strlen(message);
        int i;
        for(i=1; i < str_length; ++i) {
            if (message[i-1] == '%' && message[i] == 'd') ++num_ints;
            if (message[i-1] == '%' && message[i] == 'f') ++num_floats;
        }
        va_list args;
        va_start(args, num_ints + num_floats);
        MessageBuilder mb;
        make_debug_mb(&mb, subsystem, str_length + 4*num_ints + 6 * num_floats);
        for(i=0; i < str_length; ++i) {
            if (message[i] == '%' && message[i+1] == 'd') {
                mb_add_int(&mb, va_arg(args, int));
                ++i;
            } else if (message[i] == '%' && message[i+1] == 'f') {
                mb_add_float(&mb, va_arg(args, float));
                ++i;
            } else {
                mb_add_char(&mb, message[i]);
            }
        }
        va_end(args);
        send_computer_mb_unsafe(&mb);
    }
}

void quick_debug(unsigned char subsystem, const char* message) {
    if (should_print_debug(subsystem)) {
        MessageBuilder mb;
        make_mb(&mb, strlen(message) + 2);
        mb_add_char(&mb, 2); // message type is a string
        mb_add_char(&mb, subsystem); // identify subsystem
        mb_add_string(&mb, message); // send the actual message
        send_computer_mb(&mb);
    }
}

void make_debug_mb(MessageBuilder* mb,
                    unsigned char subsystem,
                    int size) {
    make_mb(mb, size+2);
    mb_add_char(mb, 2); // message type is a string

    mb_add_char(mb, subsystem);
}

int should_print_debug(int subsystem) {
    return getSetting(SETTINGs_DEBUG_MODES + subsystem - 1);
}

void set_debug(int subsystem, int should_work) {
    setSetting(SETTINGs_DEBUG_MODES + subsystem - 1, should_work);
}