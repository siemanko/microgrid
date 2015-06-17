#include "ethermini.h"

#include "shared/utils.h"

#include <stdlib.h>

#define MAX_MESSAGES 30

#define ETHERMINI_FRAMING_PREAMBLE        170
#define ETHERMINI_FRAMING_SOF             171
#define ETHERMINI_FRAMING_PREAMBLE_LENGTH 3


//DS:  Edit, for debugging output buffer
static float debug_num_messages_max = 0;
static float debug_num_total_messages_sent = 0;

static void debug_reset_max(float current_number){
    if(current_number > debug_num_messages_max){
        debug_num_messages_max=current_number;
    }   
}
float debug_ethermini_max_messages(){
    return debug_num_messages_max;
}


void make_ethermini(Ethermini* e,
        void (*put)(uint8_t),
        void (*on_message_callback)(Message*)) {
    e->outbound_messages =
            (CircularBuffer*)safe_malloc(sizeof(CircularBuffer));
    make_cb(e->outbound_messages, MAX_MESSAGES);
     e->inbound_messages =
            (CircularBuffer*)safe_malloc(sizeof(CircularBuffer));
    make_cb(e->inbound_messages, MAX_MESSAGES);

    e->put = put;
    e->on_message_callback = on_message_callback;
    e->state = FRAMING;
    e->state_aux = 0;
    e->msg_receive_buffer = 0;
}

uint32_t message_checksum(Message* msg) {
    uint32_t res = 0;

    res = checksum(res, &(msg->source), 1);
    res = checksum(res, &(msg->destination), 1);
    res = checksum(res, &(msg->length), 1);
    res = checksum(res, msg->content, msg->length);

    return res;
}

void ethermini_send(Ethermini* e,  Message *msg) {
    if (!cb_push(e->outbound_messages, msg)) {
        // buffer full.
        message_free(msg);
    }
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
    
    float dan_debug_current_num_messages = 0;    
    for (msg_idx = 0; msg_idx < msg->length; ++msg_idx) {
      e->put(msg->content[msg_idx]);
      dan_debug_current_num_messages++;
    }    
    debug_reset_max(dan_debug_current_num_messages);
        
    uint8_t cc[4];
    uint32_to_bytes(message_checksum(msg), cc);
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
                message_free(e->msg_receive_buffer);
                e->msg_receive_buffer = 0;
                e->msg_receive_buffer =
                        (Message*)safe_malloc(sizeof(Message));
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

        if (symbol == 0) {
            e->state_aux = 0;
            e->state = CHECKSUM;
        } else {
            e->msg_receive_buffer->content =
                (uint8_t*)safe_malloc(symbol*sizeof(uint8_t));
            e->state_aux = 0;
            e->state = CONTENT;
        }
    } else if (e->state == CONTENT) {
        // TODO(szymon): move this to the top of id ladder.
        e->msg_receive_buffer->content[e->state_aux++] = symbol;
        if (e->state_aux >= e->msg_receive_buffer->length) {
            e->state = CHECKSUM;
            e->state_aux = 0;
        }
    } else if (e->state == CHECKSUM) {
        e->checksum_buffer[e->state_aux++] = symbol;
        if (e->state_aux == 4) {
            uint32_t cc_received = bytes_to_uint32(e->checksum_buffer);
            uint32_t cc_computed = message_checksum(e->msg_receive_buffer);
            if (cc_received == cc_computed) {
                if (!cb_push(e->inbound_messages, e->msg_receive_buffer)) {
                    // buffer full.
                    message_free(e->msg_receive_buffer);
                    e->msg_receive_buffer = 0;
                }
                e->msg_receive_buffer = 0;
            } else {
                message_free(e->msg_receive_buffer);
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
        message_free(msg);
    }  
        
    while(!cb_empty(e->inbound_messages)) {
        Message* msg = (Message*)cb_popqueue(e->inbound_messages);
        e->on_message_callback(msg);
        message_free(msg);
    } 
}
