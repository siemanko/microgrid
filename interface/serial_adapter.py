import serial
import time
from Queue import Queue, Empty
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

        self.start_polling()
        self.message_callback = callback
        init_communication()
        self.recent_traffic = [None, None]
        self.outgoing_messages = []

    def reconnect(self):
        if self.s is not None:
            self.s.close()
        try:
            self.s = serial.Serial(port=self.port,
                                baudrate=9600,
                                bytesize=8,
                                stopbits=2,
                                parity='N',
                                timeout=0.00001)  # open first serial port
        except Exception:
            print 'Error connecting to %s' % (self.port)

    def start_polling(self):
        self.thread_should_stop = False
        def thread_loop():
            while not self.thread_should_stop:
                try:
                    x = [ ord(x) for x in self.s.read(1)]
                    if len(x) > 0:
                        if debug_bytes:
                            print 'INCOMING traffic: ', x[0]
                        self.recent_traffic[0] = x[0]
                        on_symbol(x[0])
                    msg = receive_message()
                    if msg is not None:
                        self.message_callback(msg)

                    outgoing_char = get_outgoing_char()
                    if outgoing_char is not None:
                        self.s.write([outgoing_char])
                        if debug_bytes:
                            print 'OUTGOING traffic: ', outgoing_char
                        self.recent_traffic[1] = outgoing_char
                    if len(self.outgoing_messages) > 0:
                        msg = self.outgoing_messages.pop()
                        send_message(msg)
                except Exception as e:
                    time.sleep(1.0)
                    print 'WARNING: Exception in serial - reconnecting...'
                    self.reconnect()


        self.polling_thread = Thread(target=thread_loop)
        self.polling_thread.setDaemon(True)
        self.polling_thread.start()

    def pop_recent_traffic(self):
        """Returns most recent byte since the last time function was called,
        or non if there was no traffic. Returns tuple (inbound, outbound)"""
        ret = self.recent_traffic
        self.recent_traffic = [None, None]
        return ret

    def send(self, msg):
        self.outgoing_messages.append(msg)

    def close(self):
        self.thread_should_stop = True
        self.polling_thread.join()
        if self.s is not None:
            self.s.close()
