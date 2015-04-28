/* 
 * File:   interface.h
 * Author: sidor
 *
 * Created on April 25, 2015, 8:11 PM
 */

#ifndef COMMUNICATION_INTERFACE_H
#define	COMMUNICATION_INTERFACE_H

#include "communication/computer.h"
#include "shared/communication/utils/message_builder.h"

#define COMPUTER ((uint8_t)254)

void init_communication();

// only works for messages with zero at the end.
// takes ownership of content.
void send_string(uint8_t* msg, uint8_t destination);

// takes ownership of content, but not MB.
void send_mb(MessageBuilder* mb, uint8_t destination);

// takes ownership of content.
void send(uint8_t* msg, uint8_t length, uint8_t destination);

void communication_step();

#endif	/* INTERFACE_H */

