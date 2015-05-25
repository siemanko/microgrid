#include <stdio.h>
#include <stdlib.h>
#include <libserialport.h>

#include "communication_wrapper.h"
#include "serial.h"
#include "shared/utils.h"

void assert_callback(const char* format, va_list args) {
    vprintf(format, args);
    printf(" (such interception)\n");
    exit(1);
}

char message_buffer[10000];

int ping_ctr = 0;
void ping() {
    int ping_msg_len = 1;
    unsigned char ping_msg[] = {0};
    printf("Sending ping no %d\n", ++ping_ctr);
    comm_send_message(ping_msg, ping_msg_len);
}

void receiving_step() {
    while(1) {
        unsigned char ret;
        if (serial_read_blocking_1ms(&ret)) {
            comm_on_symbol(ret);
        } else {
            break;
        }
    }
    int len = comm_receive_message(message_buffer);
    if (len > 0) {
        printf("Got message: ");
        int midx;
        for (midx = 0; midx < len; ++ midx)
            printf("%d ", message_buffer[midx]);
        printf("\n");
        ping();
    }
}

int main() {
    set_assert_callback(assert_callback);

    init_serial();
    init_comm();


    ping();
    while(1) {

        receiving_step();
    }

    return 0;

    /*

    int midx;
    for (midx = 0; midx < ping_msg_len; ++ midx) {
        serial_send_byte(ping_msg[midx]);
    }
    while(1) {
        unsigned char ret;
        if (serial_read_blocking_1ms(&ret)) {
            printf("Got %d\n", (int)ret);
        }
    }*/
}
