from kivy.clock import Clock

class Ctrl(object):
    def __init__(self, root):
        self.root = root

        Clock.schedule_interval(self.update_indicators, 0.2)

    def clear_logs(self):
        self.root.logs.adapter.data = []

    def send_ping(self):
        self.root.serial.send('ping')

    def update_indicators(self, *largs):
        inbound, outbound = self.root.serial.pop_recent_traffic()
        self.root.indicators.update(inbound, outbound)

get = None
