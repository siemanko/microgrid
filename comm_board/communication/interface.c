#include "interface.h"

#include <stdlib.h>
#include <string.h>

#include "communication/messages.h"
#include "shared/communication/utils/message_builder.h"

void init_communication() {
    init_computer_communication();
    register_misc_message_handlers();
}

void send_string(uint8_t* msg, uint8_t destination) {
    send(msg, strlen((char*)msg), destination);
}


void send(uint8_t* content, uint8_t length, uint8_t destination) {
    Message* msg = (Message*)malloc(sizeof(Message));
    
    msg->source = 1;
    msg->destination = destination;
    msg->length = length;
    msg->content = content;
    
    if (destination == COMPUTER) {
        computer_send_message(msg);
    } else {
        // not yet implemented
    }
}

void send_mb(MessageBuilder* mb, uint8_t destination) {
    send((uint8_t*)mb->message, mb->next_char, destination);
}


void communication_step() {
    computer_step();
}
