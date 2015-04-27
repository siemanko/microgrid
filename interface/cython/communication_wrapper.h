#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>

#include "shared/communication/message.h"
#include "shared/communication/ethermini.h"


Ethermini network;
CircularBuffer outgoing_characters;

int c_outgoing_empty() {
    return cb_empty(&outgoing_characters);
}

unsigned char c_outgoing_get() {
    return (unsigned char)cb_popqueue(&outgoing_characters);
}

void put_char(uint8_t symbol) {
    cb_push(&outgoing_characters, (void*)symbol);
}

void c_send_message(char* content) {
    Message msg;
    msg.length = strlen(content);
    msg.source = 254;
    msg.destination = 1;
    msg.content = content;
    ethermini_send_immediately(&network, &msg);
}

void on_message(Message* msg) {

}

void c_init_communication() {
    make_ethermini(&network, put_char, on_message);
    make_cb(&outgoing_characters, 10000);
}

void c_on_symbol(unsigned char symbol) {
    ethermini_on_symbol(&network, symbol);
}

char* c_receive_message() {
    Message* msg =
            (Message*)ethermini_receive_message(&network);
    if (msg) {
        char* ret = (char*)malloc(sizeof(uint8_t) *
                           (msg->length + 1));

        int i;
        for (i=0; i<msg->length; ++i) {
            ret[i] = msg->content[i];
        }
        ret[msg->length] = 0;
        return ret;
    } else {
        return 0;
    }
    //return "o hai";
}
