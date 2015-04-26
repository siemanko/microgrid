#include <stdint.h>
#include <stdlib.h>

#include <stdio.h>

#include "shared/communication/ethermini.h"


Ethermini network;

void put_char(uint8_t symbol) {

}

void c_init_communication() {
    make_ethermini(&network, put_char);
}

void c_on_symbol(unsigned char symbol) {
    printf("Symbol received => %u\n", symbol);
    ethermini_on_symbol(&network, symbol);
}

char* c_receive_message() {
    EtherminiMessage* msg =
            (EtherminiMessage*)ethermini_receive_message(&network);
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
