#include "messages.h"
#include "communication/interface.h"
#include "shared/utils.h"

void handle_message(Message* msg) {
    send_string((uint8_t*)copy_string("pong"), COMPUTER);
}