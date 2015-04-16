#include "MessageBuilder.h"

#include "ComputerCommunicationDriver.h"

void make_mb(MessageBuilder* mb, int capacity) {
    mb->next_char = 0;
    if (capacity > 0) {
        mb->message = (char*)malloc((capacity)*sizeof(char));
    } else {
        mb->message = 0;
    }
    mb->capacity = capacity;
    mb->ignore_me = 0;
}

void mb_add_char(MessageBuilder* mb, char chr) {
    if (mb->next_char < mb->capacity)
    (mb->message)[mb->next_char++] = chr;
}

void mb_add_string(MessageBuilder* mb, const char* str) {
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
    mb_cpy->ignore_me = mb->ignore_me;
    if (deep && mb->capacity > 0) {
        mb_cpy->message = (char*)malloc((mb->capacity+1)*sizeof(char));
        int i;
        for(i=0; i<mb->next_char; ++i) {
            (mb_cpy->message)[i] = (mb->message)[i];
        }
    }
    return mb_cpy;
}



void mb_add_float(MessageBuilder* mb, float f) {
    mb_add_char(mb, '%');
    mb_add_char(mb, 'f');

    mb_add_float_noprefix(mb, f);
}

void mb_add_float_noprefix(MessageBuilder* mb, float f) {
    unsigned char *ptr;
    ptr = (unsigned char *)&f;
    int i;

    for(i=0;i<=3;i++){
       mb_add_char(mb, *(char *)ptr);
       ptr++;
    }
}

void mb_add_int(MessageBuilder* mb, int d) {
    mb_add_char(mb, '%');
    mb_add_char(mb, 'd');

    mb_add_int_noprefix(mb, d);
}

void mb_add_int_noprefix(MessageBuilder* mb, int d) {
    unsigned char *ptr;
    ptr = (unsigned char *)&d;
    int i;

    for(i=0;i<=1;i++){
       mb_add_char(mb, *(char *)ptr);
       ++ptr;
    }
}
