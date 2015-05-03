from libc.stdlib cimport malloc, free

cdef extern from "communication_wrapper.h":
    cdef unsigned char* c_receive_message(int*)
    cdef void c_init_communication()
    cdef void c_on_symbol(unsigned char)
    cdef void c_send_message(char*, int)
    cdef int c_outgoing_empty()
    cdef unsigned char c_outgoing_get()

def init_communication():
    print 'c_init_communication'
    c_init_communication()
    print 'c_init_communication_end'


def on_symbol(symbol):
    cdef unsigned char c_symbol = symbol;
    print 'c_on_symbol'
    c_on_symbol(c_symbol)
    print 'c_on_symbol_end'


def receive_message():
    #print 'receive_message'
    cdef int length
    print 'c_receive_message'
    cdef unsigned char* ret = c_receive_message(&length)
    print 'c_receive_message_end'
    if ret == NULL:
        return None
    else:
        result = ret[:length]
        free(ret)
        return result

def get_outgoing_char():
    #print 'outgoing_char'
    res = None
    print 'c_get_char'
    if c_outgoing_empty() != 1:
        res = c_outgoing_get()
    print 'c_get_char_end'
    return res
def send_message(msg):
    print 'c_send start'
    c_send_message(msg, len(msg))
    print 'c_send end'
