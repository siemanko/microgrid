cdef extern from "communication_wrapper.h":
    cdef unsigned char* c_receive_message(int*)
    cdef void c_init_communication()
    cdef void c_on_symbol(unsigned char)
    cdef void c_send_message(char*, int)
    cdef int c_outgoing_empty()
    cdef unsigned char c_outgoing_get()

def init_communication():
    c_init_communication()

def on_symbol(symbol):
    cdef unsigned char c_symbol = symbol;
    c_on_symbol(c_symbol)

def receive_message():
    cdef int length
    cdef unsigned char* ret = c_receive_message(&length)
    if ret == NULL:
        return None
    else:
        return ret[:length]


def get_outgoing_char():
    if c_outgoing_empty() == 1:
        return None
    else:
        return c_outgoing_get()

def send_message(msg):
    c_send_message(msg, len(msg))
