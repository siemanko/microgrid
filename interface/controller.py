class Ctrl(object):
    def __init__(self, root):
        self.root = root

    def clear_logs(self):
        self.root.logs.adapter.data = []

get = None
