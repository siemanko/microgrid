#ifndef SHARED_ETHERMINI_H
#define SHARED_ETHERMINI_H

#include "shared/algorithm/circular_buffer.h"

#include <stdint.h>

typedef struct {
    uint8_t* content;
    uint8_t length;
    uint8_t destination;
    uint8_t source;
} EtherminiMessage;

typedef enum {
    FRAMING,  // Looking for at least two preambles followed by SOF
    DESTINATION,
    SOURCE,
    LENGTH,
    CONTENT,
    CHECKSUM,
} EtherminiState;

typedef struct {
    CircularBuffer* outbound_messages;
    CircularBuffer* inbound_messages;

    void (*put)(uint8_t);
    EtherminiState state;
    int state_aux;
    EtherminiMessage* msg_receive_buffer;
    uint8_t checksum_buffer[4];
} Ethermini;

void make_ethermini(Ethermini* e, void (*put)(uint8_t));

// takes ownership of the message.
// void ethermini_send(Ethermini* e, uint8_t* message)''

// sends message immendiately to recipient.
// does NOT take ownership of message message.
void ethermini_send_immediately(Ethermini* e, EtherminiMessage* message);

void ethermini_on_symbol(Ethermini* e, uint8_t symbol);

// returns message or 0 if no message was receive.
// returns message in the order they were received.
EtherminiMessage* ethermini_receive_message(Ethermini* e);

#endif
