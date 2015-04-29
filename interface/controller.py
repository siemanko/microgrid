import struct
import time

from kivy.clock import Clock

from messages import ToUlink, ToComputer

def ints_to_bytes(ints):
    return ''.join([ chr(x) for x in ints])

class MessageBuilder(object):
    def __init__(self, msg_type):
        self.buffer = [ chr(msg_type) ]

    def add_uint32(self, number):
        packed = list(struct.pack('<I', number))
        self.buffer += packed
    def to_bytes(self):
        return ''.join([x for x in self.buffer])

class Ctrl(object):
    def __init__(self, root):
        self.root = root
        Clock.schedule_interval(self.update_indicators, 0.2)

    def send(self, msg):
        self.root.serial.send(msg)

    def clear_logs(self):
        self.root.logs.adapter.data = []

    def send_ping(self):
        mb = MessageBuilder(ToUlink.PING)
        self.send(mb.to_bytes())

    def cron_stats(self):
        mb = MessageBuilder(ToUlink.CRON_STATS)
        self.send(mb.to_bytes())

    def get_time(self):
        mb = MessageBuilder(ToUlink.GET_TIME)
        self.send(mb.to_bytes())
    def sync_time(self):
        ts = int(time.time())
        mb = MessageBuilder(ToUlink.SET_TIME)
        mb.add_uint32(ts)
        self.send(mb.to_bytes())

    def update_indicators(self, *largs):
        inbound, outbound = self.root.serial.pop_recent_traffic()
        self.root.indicators.update(inbound, outbound)

get = None
