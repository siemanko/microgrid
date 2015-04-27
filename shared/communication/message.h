/* 
 * File:   message.h
 * Author: sidor
 *
 * Created on April 26, 2015, 9:47 PM
 */

#ifndef SHARED_COMMUNICATION_MESSAGE_H
#define	SHARED_COMMUNICATION_MESSAGE_H

#include <stdint.h>

typedef struct {
    uint8_t* content;
    uint8_t length;
    uint8_t destination;
    uint8_t source;
} Message;

#endif	/* MESSAGE_H */

