from threading import Lock
from libc.stdlib cimport malloc, free

VERBOSE = False

c_mutex = Lock()

cdef extern from "communication_wrapper.h":
    cdef unsigned char* c_receive_message(int*)
    cdef void c_init_communication()
    cdef void c_on_symbol(unsigned char)
    cdef void c_send_message(unsigned char*, int)
    cdef int c_outgoing_empty()
    cdef unsigned char c_outgoing_get()

def init_communication():
    with c_mutex:
        if VERBOSE: print 'c_init_communication'
        c_init_communication()
        if VERBOSE: print 'c_init_communication_end'


def on_symbol(symbol):
    cdef unsigned char c_symbol
    with c_mutex:
        c_symbol = symbol
        if VERBOSE: print 'c_on_symbol'
        c_on_symbol(c_symbol)
        if VERBOSE: print 'c_on_symbol_end'


def receive_message():
    #print 'receive_message'
    cdef int length
    cdef unsigned char* ret
    with c_mutex:
        if VERBOSE: print 'c_receive_message'
        ret = c_receive_message(&length)
        if VERBOSE: print 'c_receive_message_end'
        if ret == NULL:
            return None
        else:
            result = bytearray(ret[:length])
            free(ret)
            return result

def get_outgoing_char():
    with c_mutex:
        #print 'outgoing_char'
        res = None
        if VERBOSE: print 'c_get_char'
        if c_outgoing_empty() != 1:
            res = c_outgoing_get()
        if VERBOSE: print 'c_get_char_end'
        return res

def send_message(msg):
    with c_mutex:
        if VERBOSE: print 'c_send start'
        c_send_message(msg, len(msg))
        if VERBOSE: print 'c_send end'
