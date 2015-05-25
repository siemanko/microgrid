#include "serial.h"

#include <libserialport.h>

#include "shared/utils.h"

void init_serial(char* portName) {
    assert(sp_get_port_by_name(portName, &port) == SP_OK);
    assert(sp_open(port, SP_MODE_READ_WRITE) == SP_OK);

    assert(SP_OK == sp_set_baudrate(port, 57600));
    assert(SP_OK == sp_set_bits(port, 8));
    assert(SP_OK == sp_set_stopbits(port, 2));
    assert(SP_OK == sp_set_parity(port, SP_PARITY_EVEN));
    assert(SP_OK == sp_set_rts(port, SP_RTS_OFF));
    assert(SP_OK == sp_set_cts(port, SP_CTS_IGNORE));
    assert(SP_OK == sp_set_dtr(port, SP_DTR_OFF));
    assert(SP_OK == sp_set_dsr(port, SP_DSR_IGNORE));
    assert(SP_OK == sp_set_xon_xoff(port, SP_XONXOFF_DISABLED));
    assert(SP_OK == sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE));
}

void serial_send_byte(unsigned char byte) {
    unsigned char buf[1];
    buf[0] = byte;
    assert(sp_blocking_write (port, buf, 1, 0) == 1);
}

int serial_read_byte(unsigned char* ret) {
    unsigned char buf[1];
    int num_read = sp_nonblocking_read(port, buf, 1);
    if (num_read >= 1)
        *ret = buf[0];
    return num_read == 1;
}
