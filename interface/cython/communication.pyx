cdef extern from "communication_wrapper.h":
    cdef char* c_receive_message()
    cdef void c_init_communication()
    cdef void c_on_symbol(unsigned char)

def init_communication():
    c_init_communication()

def on_symbol(symbol):
    cdef unsigned char c_symbol = symbol;
    c_on_symbol(c_symbol)

def receive_message():
    cdef char* ret = c_receive_message()
    if ret == NULL:
        return None
    else:
        return str(ret)


