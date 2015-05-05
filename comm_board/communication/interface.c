#include "interface.h"

#include <stdlib.h>
#include <string.h>


#include "communication/messages.h"
#include "communication/network_utils.h"
#include "shared/communication/utils/message_builder.h"
#include "shared/utils.h"
#include "phone_line.h"
#include "drivers/eeprom.h"
#include "storage.h"

int my_uid;

void init_communication() {
    my_uid = eeprom_read_byte(STORAGE_UID);
    init_network_utils();
    init_computer_communication();
    init_phone_line_communication();
    register_misc_message_handlers();
}

void send_string(uint8_t* msg, uint8_t destination) {
    send(msg, strlen((char*)msg), destination);
}


void send(uint8_t* content, uint8_t length, uint8_t destination) {
    Message* msg = (Message*)safe_malloc(sizeof(Message));
    
    msg->source = my_uid;
    msg->destination = destination;
    msg->length = length;
    msg->content = content;
    
    if (destination == COMPUTER_UID) {
        computer_send_message(msg);
    } else {
        phone_line_send_message(msg);
    }
}

void send_mb(MessageBuilder* mb, uint8_t destination) {
    send((uint8_t*)mb->message, mb->next_char, destination);
}

void communication_step() {
    computer_step();
    phone_line_step();
}
