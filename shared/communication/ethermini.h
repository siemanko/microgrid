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

typedef struct {
    CircularBuffer* messages;
    void (*put)(uint8_t);
} Ethermini;

void make_ethermini(Ethermini* e, void (*put)(uint8_t));

// takes ownership of the message.
// void ethermini_send(Ethermini* e, uint8_t* message)''


// sends message immendiately to recipient.
// does NOT take ownership of message message.
void ethermini_send_immediately(Ethermini* e, EtherminiMessage* message);

#endif
