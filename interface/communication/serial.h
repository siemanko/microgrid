#ifndef SERIAL_H
#define SERIAL_H

struct sp_port* port;

void init_serial();;

void serial_send_byte(unsigned char byte);

int serial_read_byte(unsigned char* ret);

#endif
