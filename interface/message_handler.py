import pytz
import struct

import controller

from datetime import datetime
from messages import ToComputer, ToUlink
from utils import parse_float, parse_int, parse_uint32, stringify

def parse_message(msg):
    parsed_message = []
    i = 0
    last_open_str = 0

    def store_string_so_far():
        if last_open_str != i:
            parsed_message.append(stringify(msg[last_open_str:i]))

    while i < len(msg):
        if stringify(msg[i:(i+2)]) == '%d':
            store_string_so_far()
            int_bytes = msg[(i+2):(i+4)]
            parsed_int = parse_int(int_bytes)
            parsed_message.append(parsed_int)
            last_open_str = i + 4
            i = i + 4
        elif stringify(msg[i:(i+2)]) == '%f':
            store_string_so_far()
            float_bytes = msg[(i+2):(i+6)]
            parsed_float = parse_float(float_bytes)
            parsed_message.append(parsed_float)
            last_open_str = i + 6
            i = i + 6
        elif stringify(msg[i:(i+2)]) == '%l':
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
        self.ui_root.debug_panel.logs.adapter.data.append(msg)

    def update_if_not_focused(self, element, value):
        if not element.focused:
            element.text = value

    def handle(self, msg):
        if len(msg) == 0:
            print ('WARNING: Empty message')
            return
        msg_type = msg[0]
        if msg_type == ToComputer.DEBUG:
            # debug message
            subsystems = [
                'INFO',
                'ERROR',
                'CRON',
            ]
            if (0 > msg[1] or msg[1] >= len(subsystems)):
                print  ("WARNING: Unknown debug category: %d.." % (msg[1],))
                subsystem = 'UNKNOWN'
                print (stringify(msg[2:]))
            else:
                subsystem = subsystems[msg[1]]

            content = parse_message(msg[2:])
            content = ''.join([str(m) for m in content])
            content = '[%s] %s' % (subsystem, content)
            self.log(content)
        elif msg_type == ToComputer.GET_SETTINGS_REPLY:
            time_since_epoch_s = parse_uint32(msg[1:5])
            date = datetime.fromtimestamp(time_since_epoch_s)
            box_uid = msg[5]
            box_node_type = chr(msg[6])
            box_balance = parse_uint32(msg[7:11])
            #self.log('Time on device is ' + str(date))

            self.update_if_not_focused(self.ui_root.settings.box_time, str(date))
            self.update_if_not_focused(self.ui_root.settings.box_uid, str(box_uid))
            self.update_if_not_focused(self.ui_root.settings.box_node_type, str(box_node_type))
            self.update_if_not_focused(self.ui_root.settings.box_balance, str(box_balance))
        elif msg_type == ToComputer.DATA_LOGGER_REPLY:
            controller.get.data_logger.on_message(msg)
        else:
            print( 'WARNING: Uknown message type :', msg[0])
