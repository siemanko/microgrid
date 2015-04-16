/*
Communication with computer should be easy!
*/

#ifndef COMPUTERCOMMUNICATIONDRIVER_H
#define COMPUTERCOMMUNICATIONDRIVER_H

#include <stdlib.h>

#include "macros.h"
#include "MessageBuilder.h"

#define MAX_QUEUED_OUTGOING 100



void initComputerCommunication(int UID);

void computer_communication_step(void);
// Computer takes memory ownership of buffer, but no builder.
void send_computer_mb(MessageBuilder* mb);
// Computer takes memory ownership of buffer, but no builder.
void send_computer_mb_unsafe(MessageBuilder* mb);
//void init485Chip();
void initUART1(int UID);
void initComputerMessageReceiveInterrupt(int UID);
void putUART1(char data);
void printContentsOfCommBuffer();
void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt( void );

#endif	/* COMPUTERCOMMUNICATIONDRIVER_H */

