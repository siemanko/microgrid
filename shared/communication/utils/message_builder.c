#include "message_builder.h"

#include <stdlib.h>
#include <string.h>

#include <stdio.h>

void make_mb(MessageBuilder* mb, int capacity) {
    mb->next_char = 0;
    if (capacity > 0) {
        mb->message = (char*)malloc((capacity)*sizeof(char));
    } else {
        mb->message = 0;
    }
    mb->capacity = capacity;
}

void mb_ensure_capacity(MessageBuilder* mb, int bytes_required) {
    int space_left = mb->capacity - mb->next_char;
    if (space_left < bytes_required) {
        int new_capacity = mb->next_char + bytes_required;
        char* new_buffer = (char*)malloc(new_capacity*sizeof(char));
        int bufidx;
        for (bufidx =0; bufidx < mb->next_char; ++bufidx) {
            new_buffer[bufidx] = mb->message[bufidx];
        }
        free(mb->message);
        mb->message = new_buffer;
        mb->capacity = new_capacity;
    }
}

void mb_add_char(MessageBuilder* mb, char chr) {
    mb_ensure_capacity(mb, 1);
    (mb->message)[mb->next_char++] = chr;
}

void mb_add_string(MessageBuilder* mb, const char* str) {
    mb_ensure_capacity(mb, strlen(str));

    int index = 0;
    while(str[index]) {
        mb_add_char(mb, str[index]);
        ++index;
    }
}

MessageBuilder* mb_copy(MessageBuilder* mb, int deep) {
    MessageBuilder* mb_cpy =
            (MessageBuilder*)malloc(sizeof(MessageBuilder));
    mb_cpy->capacity = mb->capacity;
    mb_cpy->next_char = mb->next_char;
    mb_cpy->message = mb->message;
    if (deep && mb->capacity > 0) {
        mb_cpy->message = (char*)malloc((mb->capacity+1)*sizeof(char));
        int i;
        for(i=0; i<mb->next_char; ++i) {
            (mb_cpy->message)[i] = (mb->message)[i];
        }
    }
    return mb_cpy;
}


void mb_add_float_noprefix(MessageBuilder* mb, float f) {
    mb_ensure_capacity(mb, 4);

    unsigned char *ptr;
    ptr = (unsigned char *)&f;
    int i;

    for(i=0;i<=3;i++){
       mb_add_char(mb, *(char *)ptr);
       ptr++;
    }
}

void mb_add_float(MessageBuilder* mb, float f) {
    mb_ensure_capacity(mb, 6);

    mb_add_char(mb, '%');
    mb_add_char(mb, 'f');

    mb_add_float_noprefix(mb, f);
}

void mb_add_int_noprefix(MessageBuilder* mb, int d) {
    mb_ensure_capacity(mb, 2);

    unsigned char *ptr;
    ptr = (unsigned char *)&d;
    int i;

    for(i=0;i<=1;i++){
       mb_add_char(mb, *(char *)ptr);
       ++ptr;
    }
}


void mb_add_int(MessageBuilder* mb, int d) {
    mb_ensure_capacity(mb, 4);

    mb_add_char(mb, '%');
    mb_add_char(mb, 'd');

    mb_add_int_noprefix(mb, d);
}


void mb_add_formated(MessageBuilder* mb, char* format, ...) {


    va_list args;
    va_start(args, format);
    mb_add_formated_args(mb, format, args);

    va_end(args);
}

void mb_add_formated_args(MessageBuilder* mb, char* format, va_list args) {
    int num_ints = 0;
    int num_floats = 0;
    int str_length = strlen(format);
    int i;
    for(i=1; i < str_length; ++i) {
        if (format[i-1] == '%' && format[i] == 'd') ++num_ints;
        if (format[i-1] == '%' && format[i] == 'f') ++num_floats;
    }
    mb_ensure_capacity(mb, str_length + 2*num_ints + 4*num_floats);
    
    for(i=0; i < str_length; ++i) {
        if (format[i] == '%' && format[i+1] == 'd') {
            mb_add_int(mb, va_arg(args, int));
            ++i;
        } else if (format[i] == '%' && format[i+1] == 'f') {
            mb_add_float(mb, va_arg(args, float));
            ++i;
        } else { 
            mb_add_char(mb, format[i]);
        }
    }
}

