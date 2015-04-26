#include "ethermini.h"

#define MAX_MESSAGES 50

void make_ethermini(Ethermini* e, void (*put)(uint8_t)) {
    make_cb(e->messages, MAX_MESSAGES);
    e->put = put;
}

void ethermini_send_immediately(Ethermini *e, EtherminiMessage *msg) {
    // preamble - 2 time 0xaa
    int pidx;
    for (pidx=0; pidx < 2; ++pidx) {
        e->put(0xaa);
    }
    // start of frame delimiter.
    e->put(0xab);
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
