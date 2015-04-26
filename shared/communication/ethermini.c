#include "ethermini.h"
#include "shared/communication/ethermini.h"

#include <stdlib.h>

#define MAX_MESSAGES 50

#define ETHERMINI_FRAMING_PREAMBLE        170
#define ETHERMINI_FRAMING_SOF             171
#define ETHERMINI_FRAMING_PREAMBLE_LENGTH 3

void make_ethermini(Ethermini* e, void (*put)(uint8_t)) {
    e->outbound_messages =
            (CircularBuffer*)malloc(sizeof(CircularBuffer));
    make_cb(e->outbound_messages, MAX_MESSAGES);
     e->inbound_messages =
            (CircularBuffer*)malloc(sizeof(CircularBuffer));
    make_cb(e->inbound_messages, MAX_MESSAGES);

    e->put = put;
    e->state = FRAMING;
    e->state_aux = 0;
    e->msg_receive_buffer = 0;
}

void ethermini_send_immediately(Ethermini *e, EtherminiMessage *msg) {
    // preamble - 2 time 0xaa
    int pidx;
    for (pidx=0; pidx < ETHERMINI_FRAMING_PREAMBLE_LENGTH; ++pidx) {
        e->put(ETHERMINI_FRAMING_PREAMBLE);
    }
    // start of frame delimiter.
    e->put(ETHERMINI_FRAMING_SOF);
    // destination
    e->put(msg->destination);
    // source
    e->put(msg->source);
    e->put(msg->length);
    int msg_idx = 0;
    for (msg_idx = 0; msg_idx < msg->length; ++msg_idx) {
        e->put(msg->content[msg_idx]);
    }
    // TODO(szymon): checksum.
}

void ethermini_on_symbol(Ethermini* e, uint8_t symbol) {
    printf("Received %u in state %d (aux = %d)\n",
            symbol, e->state, e->state_aux);
    if (e->state == FRAMING) {
        // here state_aux determines how many preambles
        // have we seen.
        if (symbol == ETHERMINI_FRAMING_PREAMBLE) {
            // we saw one more preamble.
            ++e->state_aux;
        } else if (symbol == ETHERMINI_FRAMING_SOF) {
            // we saw start of frame. To consider it
            // a valid SoF, we must have seen at least
            // ETHERMINI_FRAMING_PREAMBLE_LENGTH preambles.
            if (e->state_aux >= ETHERMINI_FRAMING_PREAMBLE_LENGTH) {
                // got message - reserve space.
                if (e->msg_receive_buffer != 0) {
                    printf("FREE\n");
                    free(e->msg_receive_buffer);
                }
                e->msg_receive_buffer =
                        (EtherminiMessage*)malloc(sizeof(EtherminiMessage));
                e->state = DESTINATION;
            } else {
                e->state_aux = 0;
            }
        } else {
            e->state_aux = 0;
        }
    } else if (e->state == DESTINATION) {
        e->msg_receive_buffer->destination = symbol;
        e->state = SOURCE;
    } else if (e->state == SOURCE) {
        e->msg_receive_buffer->source = symbol;
        e->state = LENGTH;
    } else if (e->state == LENGTH) {
        e->msg_receive_buffer->length = symbol;
        e->msg_receive_buffer->content =
                (uint8_t*)malloc(symbol*sizeof(uint8_t));
        if (symbol == 0) {
            cb_push(e->inbound_messages, e->msg_receive_buffer);
            e->msg_receive_buffer = 0;
            e->state = FRAMING;
            e->state_aux = 0;
        } else {
            e->state_aux = 0;
            e->state = CONTENT;
        }
    } else if (e->state == CONTENT) {
        e->msg_receive_buffer->content[e->state_aux++] = symbol;
        if (e->state_aux == e->msg_receive_buffer->length) {
            printf("ADRES put: %u\n", e->msg_receive_buffer);
            cb_push(e->inbound_messages, e->msg_receive_buffer);
            e->msg_receive_buffer = 0;

            e->state = FRAMING;
            e->state_aux = 0;
        }
    }
}

EtherminiMessage* ethermini_receive_message(Ethermini* e) {
    if (!cb_empty(e->inbound_messages)) {

        EtherminiMessage* ret =
                (EtherminiMessage *)cb_popqueue(
                        e->inbound_messages);
        printf("ADRES got: %u\n", ret);
        printf("len: %u\n", ret->length);
        return ret;
    } else {
        return 0;
    }
}
