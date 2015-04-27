#ifndef DRIVERS_UART_H
#define	DRIVERS_UART_H

#include <stdint.h>

typedef enum {
    UART_DEVICE1 = 0,
    UART_DEVICE2 = 1
} uart_device;

void init_uart(uart_device u);

void uart_put(uart_device u, uint8_t data);

void uart_set_callback(uart_device u, void (*callback)(uint8_t));

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt( void );

#endif	

