#include "phone_line.h"

#include "communication/messages.h"
#include "shared/communication/ethermini.h"
#include "drivers/uart.h"

static Ethermini phone_line_network;

void put_uart2(uint8_t symbol) {
    uart_put(UART_DEVICE2, symbol);
}

void on_receive_uart2(uint8_t symbol) {
    ethermini_on_symbol(&phone_line_network, symbol);
}

void init_phone_line_communication() {
    init_uart(UART_DEVICE2);
    make_ethermini(&phone_line_network, put_uart2, handle_message);
    uart_set_callback(UART_DEVICE2, on_receive_uart2);
}

void phone_line_send_message(Message* msg) {
    ethermini_send(&phone_line_network, msg);
}

void phone_line_send_message_unsafe(Message *msg) {
    ethermini_send_immediately(&phone_line_network, msg);
}

void phone_line_step() {
    ethermini_step(&phone_line_network);
}