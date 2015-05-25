#include <stdio.h>
#include <stdlib.h>

#include "communication_wrapper.h"
#include "serial.h"
#include "shared/utils.h"

void assert_callback(const char* format, va_list args) {
    vprintf(format, args);
    printf(" (such interception)\n");
    exit(1);
}

unsigned char incoming_message_buffer[10000];
unsigned char outgoing_message_buffer[10000];

int message_length = 0;

void receiving_step() {
    while(1) {
        unsigned char ret;
        if (serial_read_blocking_1ms(&ret)) {
            comm_on_symbol(ret);
        } else {
            break;
        }
    }
    if (message_length == 0) {
        message_length = comm_receive_message(incoming_message_buffer);
    }
}


void ui_step() {
    char buf[2];
    scanf("%s", buf);
    assert(buf[1] == 0);
    if (buf[0] == 'g') {
        if (message_length > 0) {
            int midx;
            for(midx = 0; midx < message_length; ++midx) {
                printf("%d ", incoming_message_buffer[midx]);
            }
            printf("\n");
            message_length = 0;
        } else {
            printf("none\n");
        }
        fflush(stdout);
    } else if (buf[0] == 's') {
        int length;
        scanf("%d", &length);
        int rmidx;
        for(rmidx = 0; rmidx < length; ++rmidx) {
            int x;
            scanf("%d", &x);
            outgoing_message_buffer[rmidx] = (unsigned char)x;
        }
        comm_send_message(outgoing_message_buffer, length);
    } else {
        assert(0 == "Possible commands are s,g");
    }
}


int main(int argc, char** argv) {
    set_assert_callback(assert_callback);

    if (argc != 2) {
        printf("Usage: %s DEVICE\n", argv[0]);
        exit(2);
    }

    init_serial(argv[1]);
    init_comm();

    while(1) {
        ui_step();
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
