class Ctrl(object):
    def __init__(self, root):
        self.root = root

    def clear_logs(self):
        self.root.logs.adapter.data = []

    def send_ping(self):
        self.root.serial.send('ping')

get = None
