#ifndef COMMUNICATION_COMPUTER_H
#define COMMUNICATION_COMPUTER_H

#include <stdint.h>

#include "shared/communication/ethermini.h"

#define COMPUTER_UID 254

void init_computer_communication();

void computer_send_message(EtherminiMessage *msg);


#endif