/* 
 * File:   NodalCommunication.h
 * Author: Dan Strawser
 *
 * This is a package which controls communication between uLinks
 *
 * Created on October 17, 2014, 4:35 PM
 */

#ifndef NODALCOMMUNICATIONDRIVER_H
#define	NODALCOMMUNICATIONDRIVER_H

#include <stdint.h>
#include "MessageBuilder.h"

#define NODAL_BOARDCAST_ADDRESS               255

#define NODALMSG_TOKEN_OUT                       1
#define NODALMSG_TOKEN_IN                        2
#define NODALMSG_ACCEPTING_CONNECTIONS           3
#define NODALMSG_CONNECTION_REQUEST              4
#define NODALMSG_PING                            5
#define NODALMSG_PONG                            6



// Message format on the wire
//
// 1:                LENGTH
// 2:                TARGET UID
// 3:                SOURCE UID
// 4:                MESSAGE TYPE (first 16 reserved for system)
// 4:(LENGTH-4)      DATA
// (LENGTH-4):LENGTH CHECKSUM

typedef struct StructNetworkMessage {
    uint8_t len;
    uint8_t target;
    uint8_t source;
    uint8_t type;
    uint8_t* body;
} NetworkMessage;

void initNetworkCommunication();

void network_step(void);

unsigned long checksum(uint8_t * bytes,
                       int length,
                       uint8_t * header);

// Takes ownership of the internal buffer but not of message.
void SendNetworkMessage(NetworkMessage* msg);

void SendNetworkMessageRaw(char * msg);

void connected_nodes(char* ignore);

void send_ping(uint8_t uid);

void send_node_mb(uint8_t node, uint8_t type, MessageBuilder* mb);

// Takes ownership of the internal buffer but not of message.
void send_node_mb_unsafe(uint8_t node, uint8_t type, MessageBuilder* mb);

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt( void );

#endif	/* NODALCOMMUNICATION_H */

