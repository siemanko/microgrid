import struct
import time

from kivy.clock import Clock

from messages import ToUlink, ToComputer

def ints_to_bytes(ints):
    return ''.join([ chr(x) for x in ints])

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

class Ctrl(object):
    def __init__(self, root):
        self.root = root
        Clock.schedule_interval(self.update_indicators, 0.2)
        Clock.schedule_interval(self.update_settings, 1.0)

    def send(self, msg):
        self.root.serial.send(msg)

    def clear_logs(self):
        self.root.debug_panel.logs.adapter.data = []

    def send_ping(self):
        mb = MessageBuilder(ToUlink.PING)
        self.send(mb.to_bytes())

    def cron_stats(self):
        mb = MessageBuilder(ToUlink.CRON_STATS)
        self.send(mb.to_bytes())


    def update_settings(self, *largs):
        mb = MessageBuilder(ToUlink.GET_SETTINGS)
        self.send(mb.to_bytes())

    def get_memory(self):
        mb = MessageBuilder(ToUlink.GET_MEMORY)
        self.send(mb.to_bytes())

    def sync_time(self):
        ts = int(time.time())
        mb = MessageBuilder(ToUlink.SET_TIME)
        mb.add_uint32(ts)
        self.send(mb.to_bytes())

    def set_uid_node_type(self):
        mb = MessageBuilder(ToUlink.SET_UID_NODE_TYPE)
        try:
            uid = int(self.root.settings.box_uid.text)
            assert 0<= uid and uid <= 255
            node_type = self.root.settings.box_node_type.text
            node_type = node_type.upper()
            assert len(node_type) == 1 and node_type in ['A', 'B']
            node_type = ord(node_type)
            mb.add_byte(uid)
            mb.add_byte(node_type)
            self.send(mb.to_bytes())
        except Exception as e:
            print e
            print 'WARNING: invalid uid or node type value.'

    def set_balance(self):
        try:
            mb = MessageBuilder(ToUlink.SET_BALANCE)
            balance = int(self.root.settings.box_balance.text)
            mb.add_uint32(balance)
            self.send(mb.to_bytes())
        except Exception as e:
            print e
            print 'WARNING: invalid balance value.'

    def reset_pic(self):
        mb = MessageBuilder(ToUlink.RESET_PIC)
        self.send(mb.to_bytes())

    def update_indicators(self, *largs):
        inbound, outbound = self.root.serial.pop_recent_traffic()
        self.root.indicators.update(inbound, outbound)

get = None
