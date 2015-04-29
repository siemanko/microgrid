import pytz
import struct

from datetime import datetime

from messages import ToComputer, ToUlink

def parse_int(int_bytes):
    return struct.unpack('<h', int_bytes)[0]

def parse_float(float_bytes):
    return struct.unpack('<f', float_bytes)[0]

def parse_uint32(uint32_bytes):
    return struct.unpack('<I', uint32_bytes)[0]

def parse_message(msg):
    parsed_message = []
    i = 0
    last_open_str = 0

    def store_string_so_far():
        if last_open_str != i:
            parsed_message.append(msg[last_open_str:i])

    while i < len(msg):
        if msg[i:(i+2)] == '%d':
            store_string_so_far()
            int_bytes = msg[(i+2):(i+4)]
            parsed_int = parse_int(int_bytes)
            parsed_message.append(parsed_int)
            last_open_str = i + 4
            i = i + 4
        elif msg[i:(i+2)] == '%f':
            store_string_so_far()
            float_bytes = msg[(i+2):(i+6)]
            parsed_float = parse_float(float_bytes)
            parsed_message.append(parsed_float)
            last_open_str = i + 6
            i = i + 6
        elif msg[i:(i+2)] == '%l':
            store_string_so_far()
            uint32_bytes = msg[(i+2):(i+6)]
            parsed_uint32 = parse_uint32(uint32_bytes)
            parsed_message.append(parsed_uint32)
            last_open_str = i + 6
            i = i + 6
        else:
            if i+1 == len(msg):
                i += 1
                store_string_so_far()
            else:
                i += 1

    return parsed_message



class MessageHandler(object):
    def __init__(self, ui_root):
        self.ui_root = ui_root

    def log(self, msg):
        self.ui_root.logs.adapter.data.append(msg)

    def handle(self, msg):
        if len(msg) == 0:
            print 'WARNING: Empty message'
            return
        msg_type = ord(msg[0])
        if msg_type == ToComputer.DEBUG:
            # debug message
            subsystem = [
                'INFO',
                'ERROR',
                'CRON'
            ][ord(msg[1])]
            content = parse_message(msg[2:])
            content = ''.join([str(m) for m in content])
            content = '[%s] %s' % (subsystem, content)
            self.log(content)
        elif msg_type == ToComputer.GET_TIME_REPLY:
            time_since_epoch_s = parse_uint32(msg[1:5])
            date = datetime.fromtimestamp(time_since_epoch_s)
            self.log('Time on device is ' + str(date))
        else:
            print 'WARNING: Uknown message type :', ord(msg[0])
