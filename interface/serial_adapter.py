import serial
import time

from threading import Thread

from bindings.communication import (
    init_communication,
    on_symbol,
    receive_message,
    get_outgoing_char,
    send_message,
)

debug_bytes = False

# TODO(szymon): make reliable

class SerialAdapter(object):
    def __init__(self, port='/dev/ttyUSB0', callback=lambda x: None):
        self.s = None
        self.port = port
        self.reconnect()

        self.message_callback = callback
        init_communication()
        self.recent_traffic = [None, None]
        self.outgoing_messages = []
        self.thread_should_stop = False

        self.start_threads()

    def reconnect(self):
        if self.s is not None:
            self.s.close()
        try:
            self.s = serial.Serial(port=self.port,
                                baudrate=57600,
                                bytesize=8,
                                stopbits=2,
                                parity=serial.PARITY_EVEN,
                                timeout=1.0,
                                rtscts=False)  # open first serial port
        except Exception:
            print('Error connecting to %s' % (self.port))

    def start_threads(self):
        def reading_loop():
            while not self.thread_should_stop:
                try:
                    x = self.s.read(1) #[ ord(x) for x in self.s.read(1)]
                    if len(x) > 0:
                        if debug_bytes:
                            print ('INCOMING traffic: ', x[0])
                        self.recent_traffic[0] = x[0]
                        on_symbol(x[0])
                    msg = receive_message()
                    if msg is not None:

                        self.message_callback(msg)
                except Exception as e:
                    print (e)
                    print ('WARNING: Exception in serial (reading) - reconnecting...')
                    time.sleep(1.0)
                    self.reconnect()

        def writing_loop():
            while not self.thread_should_stop:
                try:
                    outgoing_char = get_outgoing_char()
                    msg = []
                    while outgoing_char is not None:
                        msg.append(outgoing_char)
                        self.recent_traffic[1] = outgoing_char
                        if debug_bytes:
                            print ('OUTGOING traffic: ', outgoing_char)
                        outgoing_char = get_outgoing_char()

                    if len(msg) > 0:
                        self.s.write(bytearray(msg))
                    else:
                        time.sleep(0.03)

                    if len(self.outgoing_messages) > 0:
                        msg = self.outgoing_messages.pop()
                        send_message(msg)
                except Exception as e:
                    print (e)
                    print ('WARNING: Exception in serial (writing) - reconnecting...')
                    time.sleep(1.0)

        self.reading_thread = Thread(target=reading_loop)
        self.reading_thread.setDaemon(True)
        self.writing_thread = Thread(target=writing_loop)
        self.writing_thread.setDaemon(True)

        self.reading_thread.start()
        self.writing_thread.start()

    def pop_recent_traffic(self):
        """Returns most recent byte since the last time function was called,
        or non if there was no traffic. Returns tuple (inbound, outbound)"""
        if hasattr(self, 'recent_traffic'):
            ret = self.recent_traffic
            self.recent_traffic = [None, None]
            return ret
        else:
            self.recent_traffic = [None, None]
            return [None, None]

    def send(self, msg):
        self.outgoing_messages.append(msg)

    def close(self):
        self.thread_should_stop = True
        self.reading_thread.join()
        self.writing_thread.join()
        if self.s is not None:
            self.s.close()
