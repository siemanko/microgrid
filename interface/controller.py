from kivy.clock import Clock

from messages import ToUlink, ToComputer

def ints_to_bytes(ints):
    return ''.join([ chr(x) for x in ints])

class Ctrl(object):
    def __init__(self, root):
        self.root = root

        Clock.schedule_interval(self.update_indicators, 0.2)

    def clear_logs(self):
        self.root.logs.adapter.data = []

    def send_ping(self):
        self.root.serial.send(ints_to_bytes([ToUlink.PING]))

    def get_time(self):
        self.root.serial.send(ints_to_bytes([ToUlink.GET_TIME]))

    def update_indicators(self, *largs):
        inbound, outbound = self.root.serial.pop_recent_traffic()
        self.root.indicators.update(inbound, outbound)

get = None
