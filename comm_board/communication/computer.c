#include "computer.h"

#include "communication/messages.h"
#include "shared/communication/ethermini.h"
#include "drivers/uart.h"

static Ethermini computer_network;

void put_uart1(uint8_t symbol) {
    uart_put(UART_DEVICE1, symbol);
}

void on_receive_uart1(uint8_t symbol) {
    ethermini_on_symbol(&computer_network, symbol);
}

void init_computer_communication() {
    init_uart(UART_DEVICE1);
    make_ethermini(&computer_network, put_uart1, handle_message);
    uart_set_callback(UART_DEVICE1, on_receive_uart1);
}

void computer_send_message(Message* msg) {
    ethermini_send(&computer_network, msg);
}

void computer_send_message_unsafe(Message *msg) {
    ethermini_send_immediately(&computer_network, msg);
}

void computer_step() {
    ethermini_step(&computer_network);
}