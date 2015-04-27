#include "computer.h"

#include "shared/communication/ethermini.h"
#include "drivers/uart.h"

Ethermini network;

void put_uart1(uint8_t symbol) {
    uart_put(UART_DEVICE1, symbol);
}

void init_computer_communication() {
    make_ethermini(&network, put_uart1);
}

void computer_send_message(Message* msg) {
    // TODO(szymon): add to queue.
    ethermini_send(&network, msg);
}

void computer_step() {
    ethermini_step(&network);
}