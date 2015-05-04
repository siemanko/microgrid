import struct

VERBOSE = False

class MessageBuilder(object):
    def __init__(self, msg_type):
        self.buffer = [ msg_type ]

    def add_byte(self, number):
        if VERBOSE: print ('add_byte')
        assert 0 <= number and number <= 255
        self.buffer.append(number)
        if VERBOSE: print ('add_byte_end')

    def add_uint32(self, number):
        if VERBOSE: print ('add_uint32')
        packed = list(struct.pack('<I', number))
        self.buffer += packed
        if VERBOSE: print ('add_uint32_end')

    def to_bytes(self):
        return bytearray([x for x in self.buffer])

def parse_int(int_bytes):
    if len(int_bytes) == 2:
        if VERBOSE: print ('parse_int')
        ret = struct.unpack('<h', int_bytes)[0]
        if VERBOSE: print ('parse_int_end')
        return ret
    else:
        return 666

def parse_float(float_bytes):
    if len(float_bytes) == 4:
        if VERBOSE: print ('parse_float')
        ret = struct.unpack('<f', float_bytes)[0]
        if VERBOSE: print ('parse_float_end')
        return ret
    else:
        return 666
def parse_uint32(uint32_bytes):
    if len(uint32_bytes) == 4:
        if VERBOSE: print ('parse_uint32')
        ret = struct.unpack('<I', uint32_bytes)[0]
        if VERBOSE: print ('parse_uint32_end')
        return ret
    else:
        return 666

def stringify(msg):
    return ''.join([chr(x) for x in msg])
