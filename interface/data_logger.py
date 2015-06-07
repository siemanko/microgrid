import time

from threading import Thread

from messages import ToUlink
from utils import MessageBuilder
from utils import parse_uint32, parse_float, stringify

VERBOSE = False

class DataLoggerMessages(object):
    GET_STATUS        =  1
    EXTRACT_GENERAL   =  2
    EXTRACT_COLUMN    =  3
    EXTRACT_DATA      =  4
    RESET             =  5

class ColumnType(object):
    FLOAT = 1
    UINT32 = 2

class DataPuller(object):

    def __init__(self, send, log ):
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

        self.number_of_data_points_received_in_current_batch = 0     #DS:  Edit, the current line you are pulling from
        self.this_many_readings_between_saves = 4   #DS:  Edit

        #DS:  EDIT:  Might want to put entry point here
        f1 = open('LastDataPointSeen.txt','r')
        self.position_of_last_end = f1.readline()
        self.number_of_total_data_points_received = self.position_of_last_end
        f1.close()

    def on_message(self, msg):
        try:
            print('48 data logger')
            dl_msg_type = msg[1]
            if dl_msg_type == DataLoggerMessages.EXTRACT_GENERAL:       #DS:  identifies the general information about the data
                self.uid = msg[2]
                self.n_columns = parse_uint32(msg[3:7])
                self.n_entries = parse_uint32(msg[7:11])
                self.schema_name = stringify(msg[11:])
                print('55 data logger schema name : ' + self.schema_name)
            elif dl_msg_type == DataLoggerMessages.EXTRACT_COLUMN:      #DS:  which columns contain which data
                column_idx = msg[2]
                self.column_done[column_idx] = True
                self.column_type[column_idx] = msg[3]
                self.column_name[column_idx] = stringify(msg[4:])
                print('61 data logger one column name : ' + self.column_name[column_idx])
            elif dl_msg_type == DataLoggerMessages.EXTRACT_DATA:    #ds:  actually getting the data points
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
                print('71 the entry idx : ' + str(entry_idx))
                print('72 data logger one entry : ' + str(tuple(entry)))
                self.entry[entry_idx] = tuple(entry)

                #DS:  Edit, For keeping track how many data points we've received so far
                self.number_of_data_points_received_in_current_batch+=1
                self.number_of_total_data_points_received+=1
                if self.number_of_total_data_points_received == self.n_entries:
                    self.number_of_total_data_points_received = 0
                    self.number_of_data_points_received_in_current_batch = 0

                self.record_number_of_data_points_received()

            if (self.schema_name is not None and
                    all(self.column_done) and
                    all(self.entry_done)):
                self.done = True

            print('85 num data points received in current batch: ' + str(self.number_of_data_points_received_in_current_batch))
            print('93 num data points received total : ' +str(self.number_of_total_data_points_received))
            print('91 num of entries :  ' + str(self.n_entries))
            #DS:  Edit
            if self.number_of_data_points_received_in_current_batch  > self.this_many_readings_between_saves:
                print('83 data logger')
                self.partial_save_data()
                self.number_of_data_points_received_in_current_batch=0


        except Exception as e:
            print(e)
            self.failure = True

    def start(self):
        self.thread = Thread(target=self.pull)
        self.thread.setDaemon(True)
        self.thread.start()

    def reset_data_logger(self):
        fname = "LastDataPointSeen.txt"
        f=open(fname,'w')
        f.write(str(0))

    def stop(self):
        self.please_stop = True
        self.thread.join()

    def pull(self):
        MSG_TIMEOUT = .5  #DS:  Edit, used to be 0.0001
        BATCH_TIMEOUT = 1
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


        for batch_start in range(int(self.position_of_last_end), self.n_entries, BATCH_SIZE):

            print(' at 155 batch starting again!  this is batch start : ' + str(batch_start))

            if VERBOSE: print ('batch_start')

            batch_end = min(batch_start + BATCH_SIZE, self.n_entries)

            while not all(self.entry_done[batch_start:batch_end]) and not self.please_stop:
                for entry in range(batch_start, batch_end):     #it's right here.
                    print(' 163 this is entry :  ' + str(entry) + ' and this is batch start : ' + str(batch_start) + ' and thsi is batch end : ' + str(batch_end))

                    if not self.entry_done[entry]:
                        if VERBOSE: print ('msg_start')
                        mb = MessageBuilder(ToUlink.DATA_LOGGER)
                        mb.add_byte(DataLoggerMessages.EXTRACT_DATA)
                        mb.add_uint32(entry)
                        self.send(mb.to_bytes())
                        if VERBOSE: print ('msg_end')
                        time.sleep(MSG_TIMEOUT)
                time.sleep(BATCH_TIMEOUT)

            for entry in range(batch_start, batch_end):
                self.log(self.entry_line(entry))
            if VERBOSE: print ('batch_end')
    def general_info(self):
        return [
            '# schema: %s' % self.schema_name,
            '# uid: %d' % self.uid,
        ]

    def column_line(self):
        return ','.join(self.column_name)

    def entry_line(self, entry_idx):
        if self.entry[entry_idx] is None:
            print('Warning at 171 data logger, none type!')
            return 'None Type'
        else:
            return ','.join([str(x) for x in self.entry[entry_idx]])

    def content(self):      #DS Comment, only called when saving data
        content = self.general_info()
        content.append(self.column_line())
        print('168 data logger')
        print('169 n entries ' + str(self.n_entries))

        for entry in range(self.n_entries):
            print('171 data logger in loop entry :  ' +str(entry))
            print('this is entry line : ' +self.entry_line(entry))
            content.append(self.entry_line(entry))
        print('182 data logger')
        return content

    #DS Comment, used to partially save the data in case of crash
    def partial_save_data(self):
        print('199 PRE data logger')
        lines = self.content()  #DS:  This is defined directly above
        print('172 data logger')
        lines = ['%s\n' % (line,) for line in lines]
        print('203 data logger')
        fname = "Logs\%s_partial_logs_starting_at_%s.txt" % (self.uid,self.position_of_last_end,)

        with open(fname, "w") as f:
            f.writelines(lines)
            #f.writelines('total number received : ' + str(self.number_of_total_data_points_received))
        self.log("Saved to %s" % (fname,))

    def record_number_of_data_points_received(self):
        fname = "LastDataPointSeen.txt"
        f=open(fname,'w')
        f.write(str(self.number_of_total_data_points_received))
        #f.write(self.number_of_data_points_received)
        #f.close()

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

    def reset(self):
        mb = MessageBuilder(ToUlink.DATA_LOGGER)
        mb.add_byte(DataLoggerMessages.RESET)
        self.send(mb.to_bytes())

    def load_data(self):
        self.clear_logs()
        self.log('Starting to pull the logs.')
        self.data_puller = DataPuller( self.send, self.log )
        self.data_puller.start()

    def reset_data_logger(self):
        self.clear_logs()
        self.log('Resetting Data Logger.')
        self.data_puller.reset_data_logger()

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
                    print ('pulling failed')
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
        print ('Usage: %s <device>' % (sys.argv[0],))
        sys.exit(1)

    def log(msg):
        print (msg)

    d = DataPuller(None, log)

    def msg_callback(msg):
        if msg[0] == ToComputer.DATA_LOGGER_REPLY:
            d.on_message(msg)
            if d.done:
                print( 'done')
    s = SerialAdapter(sys.argv[1], msg_callback)
    d.send = s.send

    d.start()
    while True:
        time.sleep(1.0)
