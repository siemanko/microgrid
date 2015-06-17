import time
import sys
from collections import deque
from os.path import dirname, realpath, join
from threading import Thread
from subprocess import PIPE, Popen

SCRIPT_DIR = dirname(realpath(__file__))
COMMUNICATION_BIN = join(SCRIPT_DIR, "communication", "communication")

print(COMMUNICATION_BIN)

class SerialBindings(object):
    def __init__(self, port, msg_callback):
        self.port = port
        self.start_process()
        self.thread_should_stop = False
        self.callback = msg_callback
        self.outgoing_messages = deque()
        self.thread = Thread(target=self.thread_loop)
        self.thread.setDaemon(True)
        self.thread.start()
        self.indicators = [None, None]

    def start_process(self):
        self.p = Popen([COMMUNICATION_BIN, str(self.port)], bufsize=1, stdin=PIPE, stdout=PIPE, close_fds=sys.platform!='win32')
        (self.child_stdout, self.child_stdin) = (self.p.stdout, self.p.stdin)

    def thread_loop(self):
        while not self.thread_should_stop:
            try:
                self.p.poll()
                if self.p.returncode is not None:
                    print("Crash detected.. restarting")
                    self.start_process()
                while len(self.outgoing_messages) > 0:
                    msg = self.outgoing_messages.popleft()
                    msg = ['s', str(len(msg))] + [str(x) for x in msg]
                    msg = ' '.join(msg) + '\n'
                    msg = msg.encode('ascii')
                    self.child_stdin.write(msg)
                    self.indicators[1] = ' :-)'
                self.child_stdin.write(b'g\n')
                self.child_stdin.flush()
                reply = self.child_stdout.readline()[:-1]
                #if(reply.decode("ascii")[0] != 'n'):
                    #print('reply 1')
                    #print('reply : ' + reply.decode("ascii"))
                time.sleep(0.1)
                if not b"none" in reply:
                    self.callback([ int(x) for x in reply.split(b' ') if (x != b'' and x!=b'\r')]);
                    self.indicators[0] = ' :-)'
            except Exception as e:
                time.sleep(1.0)
                print ("Caugth exception restarting: ", e)
                self.start_process()

    def send(self, msg):
        self.outgoing_messages.append(msg)

    def pop_recent_traffic(self):
        ret = self.indicators
        self.indicators = [None, None]
        return ret

    def close(self):
        self.thread_should_stop = True
        self.thread.join()
        self.p.kill()


if __name__ == '__main__':
    def msg_callback(x):
        assert x == [0, 0, 112, 111, 110, 103]
        print(x)
    serial = SerialBindings('/dev/ttyUSB0', msg_callback)
    try:
        while True:
            serial.send([0])
            time.sleep(0.5)
    finally:
        serial.close()

