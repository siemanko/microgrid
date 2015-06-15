#ifndef SHARED_ETHERMINI_H
#define SHARED_ETHERMINI_H

#include "shared/algorithm/circular_buffer.h"
#include "shared/communication/message.h"
#include <stdint.h>


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
    void (*on_message_callback)(Message*);
    EtherminiState state;
    uint16_t state_aux;
    Message* msg_receive_buffer;
    uint8_t checksum_buffer[4];
} Ethermini;

void make_ethermini(Ethermini* e,
                    void (*send_char_function)(uint8_t),
                    void (*on_message_callback)(Message*));

// takes ownership of the message.
void ethermini_send(Ethermini* e, Message* message);

// sends message immediately to recipient.
// does NOT take ownership of message message.
void ethermini_send_immediately(Ethermini* e, Message* message);

void ethermini_on_symbol(Ethermini* e, uint8_t symbol);

// returns message or 0 if no message was received.
// returns message in the order they were received.
Message* ethermini_receive_message(Ethermini* e);

void ethermini_step(Ethermini* e);

//DS:  Edit,  for debugging output buffer
float debug_ethermini_max_messages();


#endif
