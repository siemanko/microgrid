#ifndef COMMUNICATION_PHONE_LINE_H
#define COMMUNICATION_PHONE_LINE_H

#include "shared/communication/message.h"

void init_phone_line_communication();

void phone_line_send_message(Message *msg);

void phone_line_send_message_unsafe(Message *msg);

void phone_line_step(); 

#endif