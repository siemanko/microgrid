#include "interface.h"

void init_communication() {
    init_computer_communication();
}

void test_comm() {
    EtherminiMessage msg;
    uint8_t* m = (uint8_t*)"siema";
    msg.content = m;
    msg.length = 5;
    msg.destination = COMPUTER_UID;
    msg.source = 1;
    computer_send_message(&msg);
}