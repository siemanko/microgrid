import serial
import time

from threading import Thread

from bindings.communication import (
    init_communication,
    on_symbol,
    receive_message,
)

# TODO(szymon): make reliable

class SerialAdapter(object):
    def __init__(self, port='/dev/ttyUSB0', callback=lambda x: None):
        self.s = serial.Serial(port='/dev/ttyUSB0',
                            baudrate=57600,
                            bytesize=8,
                            stopbits=1,
                            parity='N',
                            timeout=1)  # open first serial port

        self.start_polling()
        self.message_callback = callback
        init_communication()

    def start_polling(self):
        self.thread_should_stop = False
        def thread_loop():
            while not self.thread_should_stop:
                x = [ ord(x) for x in self.s.read(1)]
                if len(x) > 0:
                    on_symbol(x[0])
                msg = receive_message()

                if msg is not None:
                    self.message_callback(msg)

        self.polling_thread = Thread(target=thread_loop)
        self.polling_thread.setDaemon(True)
        self.polling_thread.start()


    def close():
        self.thread_should_stop = True
        self.polling_thread.stop()
        self.s.close()
