#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>

#include "serial.h"
#include "shared/communication/message.h"
#include "shared/communication/ethermini.h"


Ethermini network;

void comm_send_message(unsigned char* content, int length) {
    Message msg;
    msg.length = length;
    msg.source = 254;
    msg.destination = 255;
    unsigned char* buffer = (unsigned char*)malloc(sizeof(char) * length);
    int i;
    for (i=0; i<length; ++i) {
        buffer[i] = content[i];
    }
    msg.content = buffer;
    ethermini_send_immediately(&network, &msg);
    free(buffer);
}

void on_message(Message* msg) {
    printf("Got message");
}

void put_char(uint8_t symbol) {
    serial_send_byte(symbol);
}


void comm_on_symbol(unsigned char symbol) {
    ethermini_on_symbol(&network, symbol);
}

void init_comm() {
    make_ethermini(&network, put_char, on_message);
}

int comm_receive_message(char* buf) {
    Message* msg =
            (Message*)ethermini_receive_message(&network);
    if (msg) {
        int i;
        for (i=0; i<msg->length; ++i) {
            buf[i] = msg->content[i];
        }
        buf[msg->length] = 0;

        return msg->length;
    } else {
        return 0;
    }
    //return "o hai";
}
