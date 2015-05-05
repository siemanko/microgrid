#ifndef COMMUNICATION_NETWORK_UTILS_H
#define COMMUNICATION_NETWORK_UTILS_H

#include <stdint.h>

#include "shared/algorithm/map.h"

extern Map nodes_online;

void init_network_utils();

void ping(uint8_t target);

void discover_nodes();

#endif