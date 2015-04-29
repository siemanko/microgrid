#ifndef COMMUNICATION_COMPUTER_H
#define COMMUNICATION_COMPUTER_H
 
#include <stdint.h>

#include "shared/communication/ethermini.h"
#include "shared/communication/message.h"


void init_computer_communication();

void computer_send_message(Message *msg);

void computer_send_message_unsafe(Message *msg);

void computer_step();

#endif