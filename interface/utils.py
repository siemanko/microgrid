import struct

class MessageBuilder(object):
    def __init__(self, msg_type):
        self.buffer = [ chr(msg_type) ]

    def add_byte(self, number):
        assert 0 <= number and number <= 255
        self.buffer.append(chr(number))

    def add_uint32(self, number):
        packed = list(struct.pack('<I', number))
        self.buffer += packed
    def to_bytes(self):
        return ''.join([x for x in self.buffer])

def parse_int(int_bytes):
    if len(int_bytes) == 2:
        return struct.unpack('<h', int_bytes)[0]
    else:
        return 666

def parse_float(float_bytes):
    if len(float_bytes) == 4:
        return struct.unpack('<f', float_bytes)[0]
    else:
        return 666
def parse_uint32(uint32_bytes):
    if len(uint32_bytes) == 4:
        return struct.unpack('<I', uint32_bytes)[0]
    else:
        return 666

