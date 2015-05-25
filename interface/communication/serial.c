#include "serial.h"

#include <stdlib.h>

#include "rs232.h"
#include "shared/utils.h"

int port_name_to_number(const char* name) {
    if (name[0] == 'C' && name[1] == 'O' && name[2] == 'M') {
        return atoi(name + 3) - 1;
    }
    if (strcmp(name, "/dev/ttyS0") == 0) return 0;
    if (strcmp(name, "/dev/ttyS1") == 0) return 1;
    if (strcmp(name, "/dev/ttyS2") == 0) return 2;
    if (strcmp(name, "/dev/ttyS3") == 0) return 3;
    if (strcmp(name, "/dev/ttyS4") == 0) return 4;
    if (strcmp(name, "/dev/ttyS5") == 0) return 5;
    if (strcmp(name, "/dev/ttyS6") == 0) return 6;
    if (strcmp(name, "/dev/ttyS7") == 0) return 7;
    if (strcmp(name, "/dev/ttyS8") == 0) return 8;
    if (strcmp(name, "/dev/ttyS9") == 0) return 9;
    if (strcmp(name, "/dev/ttyS10") == 0) return 10;
    if (strcmp(name, "/dev/ttyS11") == 0) return 11;
    if (strcmp(name, "/dev/ttyS12") == 0) return 12;
    if (strcmp(name, "/dev/ttyS13") == 0) return 13;
    if (strcmp(name, "/dev/ttyS14") == 0) return 14;
    if (strcmp(name, "/dev/ttyS15") == 0) return 15;
    if (strcmp(name, "/dev/ttyUSB0") == 0) return 16;
    if (strcmp(name, "/dev/ttyUSB1") == 0) return 17;
    if (strcmp(name, "/dev/ttyUSB2") == 0) return 18;
    if (strcmp(name, "/dev/ttyUSB3") == 0) return 19;
    if (strcmp(name, "/dev/ttyUSB4") == 0) return 20;
    if (strcmp(name, "/dev/ttyUSB5") == 0) return 21;
    if (strcmp(name, "/dev/ttyAMA0") == 0) return 22;
    if (strcmp(name, "/dev/ttyAMA1") == 0) return 23;
    if (strcmp(name, "/dev/ttyACM0") == 0) return 24;
    if (strcmp(name, "/dev/ttyACM1") == 0) return 25;
    if (strcmp(name, "/dev/rfcomm0") == 0) return 26;
    if (strcmp(name, "/dev/rfcomm1") == 0) return 27;
    if (strcmp(name, "/dev/ircomm0") == 0) return 28;
    if (strcmp(name, "/dev/ircomm1") == 0) return 29;
    assert(0 == "Port not known");
    return -1;
}


void init_serial(char* portName) {
    int port_no = port_name_to_number(portName);
    assert(RS232_OpenComport(port_no, 57600, "8E2") == 0);
}

void serial_send_byte(unsigned char byte) {
    assert(RS232_SendByte(16, byte) == 0);
}

int serial_read_byte(unsigned char* ret) {
    unsigned char buf[1];
    int num_read = RS232_PollComport(16, buf, 1);
    if (num_read >= 1)
        *ret = buf[0];
    return num_read == 1;
}
