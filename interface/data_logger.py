import time

from threading import Thread

from messages import ToUlink
from utils import MessageBuilder
from utils import parse_uint32, parse_float

VERBOSE = False

def stringify(msg):
    return ''.join([chr(x) for x in msg])

class DataLoggerMessages(object):
    GET_STATUS        =  1
    EXTRACT_GENERAL   =  2
    EXTRACT_COLUMN    =  3
    EXTRACT_DATA      =  4

class ColumnType(object):
    FLOAT = 1
    UINT32 = 2

class DataPuller(object):
    def __init__(self, send, log):
        self.send = send
        self.log = log

        self.schema_name = None
        self.uid = None
        self.n_columns = None
        self.n_entries = None
        self.column_done = [False]
        self.column_type = []
        self.column_name = []
        self.entry_done = [False]
        self.entry = []

        self.please_stop = False

        self.done = False
        self.failure = False

    def on_message(self, msg):
        try:
            dl_msg_type = msg[1]
            if dl_msg_type == DataLoggerMessages.EXTRACT_GENERAL:
                self.uid = msg[2]
                self.n_columns = parse_uint32(msg[3:7])
                self.n_entries = parse_uint32(msg[7:11])
                self.schema_name = stringify(msg[11:])
            elif dl_msg_type == DataLoggerMessages.EXTRACT_COLUMN:
                column_idx = msg[2]
                self.column_done[column_idx] = True
                self.column_type[column_idx] = msg[3]
                self.column_name[column_idx] = stringify(msg[4:])
            elif dl_msg_type == DataLoggerMessages.EXTRACT_DATA:
                entry_idx = parse_uint32(msg[2:6])
                self.entry_done[entry_idx] = True
                entry = []
                offset = 6
                for column_type in self.column_type:
                    if column_type == ColumnType.FLOAT:
                        entry.append(parse_float(msg[offset:(offset+4)]))
                        offset += 4
                    if column_type == ColumnType.UINT32:
                        entry.append(parse_uint32(msg[offset:(offset+4)]))
                        offset += 4
                self.entry[entry_idx] = tuple(entry)

            if (self.schema_name is not None and
                    all(self.column_done) and
                    all(self.entry_done)):
                self.done = True
        except Exception as e:
            print e
            self.failure = True

    def start(self):
        self.thread = Thread(target=self.pull)
        self.thread.setDaemon(True)
        self.thread.start()

    def stop(self):
        self.please_stop = True
        self.thread.join()

    def pull(self):
        MSG_TIMEOUT = 0.1
        BATCH_TIMEOUT = 0.5
        BATCH_SIZE = 4

        while self.schema_name is None and not self.please_stop:
            mb = MessageBuilder(ToUlink.DATA_LOGGER)
            mb.add_byte(DataLoggerMessages.EXTRACT_GENERAL)
            self.send(mb.to_bytes())
            time.sleep(BATCH_TIMEOUT)

        self.log("Found %d entries (in %d columns)" % (self.n_entries, self.n_columns))

        for line in self.general_info():
            self.log(line)

        self.column_done = [False for _ in range(self.n_columns)]
        self.column_name = [None for _ in range(self.n_columns)]
        self.column_type = [None for _ in range(self.n_columns)]

        while not all(self.column_done) and not self.please_stop:
            for column in range(self.n_columns):
                if not self.column_done[column]:
                    mb = MessageBuilder(ToUlink.DATA_LOGGER)
                    mb.add_byte(DataLoggerMessages.EXTRACT_COLUMN)
                    mb.add_byte(column)
                    self.send(mb.to_bytes())
                    time.sleep(MSG_TIMEOUT)
            time.sleep(BATCH_TIMEOUT)

        self.log(self.column_line())

        self.entry_done = [False for _ in range(self.n_entries)]
        self.entry = [None for _ in range(self.n_entries)]

        for batch_start in range(0, self.n_entries, BATCH_SIZE):
            if VERBOSE: print 'batch_start'
            batch_end = min(batch_start + BATCH_SIZE, self.n_entries)
            while not all(self.entry_done[batch_start:batch_end]) and not self.please_stop:
                for entry in range(batch_start, batch_end):
                    if not self.entry_done[entry]:
                        if VERBOSE: print 'msg_start'
                        mb = MessageBuilder(ToUlink.DATA_LOGGER)
                        mb.add_byte(DataLoggerMessages.EXTRACT_DATA)
                        mb.add_uint32(entry)
                        self.send(mb.to_bytes())
                        if VERBOSE: print 'msg_end'
                        time.sleep(MSG_TIMEOUT)
                time.sleep(BATCH_TIMEOUT)

            for entry in range(batch_start, batch_end):
                self.log(self.entry_line(entry))
            if VERBOSE: print 'batch_end'
    def general_info(self):
        return [
            '# schema: %s' % self.schema_name,
            '# uid: %d' % self.uid,
        ]

    def column_line(self):
        return ','.join(self.column_name)

    def entry_line(self, entry_idx):
        return ','.join([str(x) for x in self.entry[entry_idx]])

    def content(self):
        content = self.general_info()
        content.append(self.column_line())
        for entry in range(self.n_entries):
            content.append(self.entry_line(entry))
        return content

class DataLogger(object):
    def __init__(self, ui_root):
        self.ui_root = ui_root

    def clear_logs(self):
        self.ui_root.data_logger.data_logs.adapter.data = []

    def log(self, msg):
        self.ui_root.data_logger.data_logs.adapter.data.append(msg)

    def send(self, msg):
        if self.ui_root.serial:
            self.ui_root.serial.send(msg)

    def status(self):
        mb = MessageBuilder(ToUlink.DATA_LOGGER)
        mb.add_byte(DataLoggerMessages.GET_STATUS)
        self.send(mb.to_bytes())

    def load_data(self):
        self.clear_logs()
        self.log('Starting to pull the logs.')
        self.data_puller = DataPuller(self.send, self.log)
        self.data_puller.start()

    def save_data(self):
        lines = self.data_puller.content()
        lines = ['%s\n' % (line,) for line in lines]
        fname = "%s_logs.txt" % (self.data_puller.uid,)
        with open(fname, "w") as f:
            f.writelines(lines)
        self.log("Saved to %s" % (fname,))

    def on_message(self, msg):
        if self.data_puller is not None:
            self.data_puller.on_message(msg)
            if self.data_puller.done:
                self.data_puller.stop()
                if self.data_puller.failure:
                    print 'pulling failed'
                else:
                    self.save_data()
                self.data_puller = None



if __name__ == '__main__':
    import sys
    import time
    from messages import ToComputer
    from serial_adapter import SerialAdapter

    import faulthandler
    faulthandler.enable()

    if len(sys.argv) != 2:
        print 'Usage: %s <device>' % (sys.argv[0],)
        sys.exit(1)

    def log(msg):
        print msg

    d = DataPuller(None, log)

    def msg_callback(msg):
        if msg[0] == ToComputer.DATA_LOGGER_REPLY:
            d.on_message(msg)
            if d.done:
                print 'done'
    s = SerialAdapter(sys.argv[1], msg_callback)
    d.send = s.send

    d.start()
    while True:
        time.sleep(1.0)
