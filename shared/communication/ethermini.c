#include "ethermini.h"

#include "shared/utils.h"

#include <stdlib.h>

#define MAX_MESSAGES 50

#define ETHERMINI_FRAMING_PREAMBLE        170
#define ETHERMINI_FRAMING_SOF             171
#define ETHERMINI_FRAMING_PREAMBLE_LENGTH 3

void make_ethermini(Ethermini* e,
        void (*put)(uint8_t),
        void (*on_message_callback)(Message*)) {
    e->outbound_messages =
            (CircularBuffer*)malloc(sizeof(CircularBuffer));
    make_cb(e->outbound_messages, MAX_MESSAGES);
     e->inbound_messages =
            (CircularBuffer*)malloc(sizeof(CircularBuffer));
    make_cb(e->inbound_messages, MAX_MESSAGES);

    e->put = put;
    e->on_message_callback = on_message_callback;
    e->state = FRAMING;
    e->state_aux = 0;
    e->msg_receive_buffer = 0;
}

uint32_t checksum(Message* msg) {
    uint32_t res = 0;

    res = res*333 + msg->source;
    res = res*333 + msg->destination;
    res = res*333 + msg->length;

    int midx;
    for (midx = 0; midx < msg->length; ++midx) {
        res = res*333 + msg->content[midx];
    }
    return res;
}

void ethermini_send(Ethermini* e,  Message *msg) {
    cb_push(e->outbound_messages, msg);
}

void ethermini_send_immediately(Ethermini *e, Message *msg) {
    // preamble - 2 time 170
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
    uint8_t cc[4];
    uint32_to_bytes(checksum(msg), cc);
    int ccidx;
    for (ccidx = 0; ccidx < 4; ++ccidx) {
        e->put(cc[ccidx]);
    }
}

void ethermini_on_symbol(Ethermini* e, uint8_t symbol) {
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
                    free(e->msg_receive_buffer);
                }
                e->msg_receive_buffer =
                        (Message*)malloc(sizeof(Message));
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
            e->state_aux = 0;
            e->state = CHECKSUM;
        } else {
            e->state_aux = 0;
            e->state = CONTENT;
        }
    } else if (e->state == CONTENT) {
        // TODO(szymon): move this to the top of id ladder.
        e->msg_receive_buffer->content[e->state_aux++] = symbol;
        if (e->state_aux == e->msg_receive_buffer->length) {
            e->state = CHECKSUM;
            e->state_aux = 0;
        }
    } else if (e->state == CHECKSUM) {
        e->checksum_buffer[e->state_aux++] = symbol;
        if (e->state_aux == 4) {
            uint32_t cc_received = bytes_to_uint32(e->checksum_buffer);
            uint32_t cc_computed = checksum(e->msg_receive_buffer);
            if (cc_received == cc_computed) {
                cb_push(e->inbound_messages, e->msg_receive_buffer);
                e->msg_receive_buffer = 0;
            } else {
                free(e->msg_receive_buffer);
                e->msg_receive_buffer = 0;
            }
            e->state_aux = 0;
            e->state = FRAMING;
        }
    }
}

Message* ethermini_receive_message(Ethermini* e) {
    if (!cb_empty(e->inbound_messages)) {

        Message* ret =
                (Message *)cb_popqueue(
                        e->inbound_messages);
        return ret;
    } else {
        return 0;
    }
}

void ethermini_step(Ethermini* e) {
    // send all the messages.
    while(!cb_empty(e->outbound_messages)) {
        Message* msg = (Message*)cb_popqueue(e->outbound_messages);
        ethermini_send_immediately(e, msg);
        free(msg);
    }
    
    while(!cb_empty(e->inbound_messages)) {
        Message* msg = (Message*)cb_popqueue(e->inbound_messages);
        e->on_message_callback(msg);
        free(msg);
    }
}