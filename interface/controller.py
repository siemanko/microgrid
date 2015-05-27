import struct
import time

from kivy.clock import Clock

from data_logger import DataLogger
from messages import ToUlink, ToComputer
from utils import MessageBuilder

class Ctrl(object):
    def __init__(self, root):
        self.ui_root = root
        self.data_logger = DataLogger(self.ui_root)

        Clock.schedule_interval(self.update_indicators, 0.2)
        Clock.schedule_interval(self.update_settings, 1.0)
        Clock.schedule_interval(self.ui_root.update_serial_choices, 1.0)

    def send(self, msg):
        if self.ui_root.serial:
            self.ui_root.serial.send(msg)

    def clear_logs(self):
        self.ui_root.debug_panel.logs.adapter.data = []

    def send_ping(self):
        mb = MessageBuilder(ToUlink.PING)
        self.send(mb.to_bytes())

    def test_leds(self):
        mb = MessageBuilder(ToUlink.TEST_LEDS)
        self.send(mb.to_bytes())

    def print_local_time(self):
        mb = MessageBuilder(ToUlink.PRINT_LOCAL_TIME)
        self.send(mb.to_bytes())

    def cron_stats(self):
        mb = MessageBuilder(ToUlink.CRON_STATS)
        self.send(mb.to_bytes())


    def update_settings(self, *largs):
        mb = MessageBuilder(ToUlink.GET_SETTINGS)
        self.send(mb.to_bytes())

    def get_memory(self):
        mb = MessageBuilder(ToUlink.GET_MEMORY)
        self.send(mb.to_bytes())

    def sync_time(self):
        ts = int(time.time())
        mb = MessageBuilder(ToUlink.SET_TIME)
        mb.add_uint32(ts)
        self.send(mb.to_bytes())

    def set_uid_node_type(self):
        mb = MessageBuilder(ToUlink.SET_UID_NODE_TYPE)
        try:
            uid = int(self.ui_root.settings.box_uid.text)
            assert 0<= uid and uid <= 255
            node_type = self.ui_root.settings.box_node_type.text
            node_type = node_type.upper()
            assert len(node_type) == 1 and node_type in ['A', 'B']
            node_type = ord(node_type)
            mb.add_byte(uid)
            mb.add_byte(node_type)
            self.send(mb.to_bytes())
        except Exception as e:
            print (e)
            print ('WARNING: invalid uid or node type value.')

    def set_balance(self):
        try:
            mb = MessageBuilder(ToUlink.SET_BALANCE)
            balance = int(self.ui_root.settings.box_balance.text)
            mb.add_uint32(balance)
            self.send(mb.to_bytes())
        except Exception as e:
            print (e)
            print ('WARNING: invalid balance value.')

    def reset_pic(self):
        mb = MessageBuilder(ToUlink.RESET_PIC)
        self.send(mb.to_bytes())

    def get_connected_nodes(self):
        mb = MessageBuilder(ToUlink.GET_CONNECTED_NODES)
        self.send(mb.to_bytes())

    def read_eeprom(self, mem_type):
        mem_types = [ 'float', 'uint32', 'byte']
        assert mem_type in mem_types
        mem_type_idx = mem_types.index(mem_type)
        addr = None
        try:
            addr = int(self.ui_root.debug_panel.eeprom_addr.text)
        except Exception as e:
            print (e)
            print ('WARNING: invalid eeprom_addr')

        mb = MessageBuilder(ToUlink.READ_EEPROM)
        mb.add_byte(mem_type_idx)
        mb.add_uint32(addr)
        self.send(mb.to_bytes())

    def update_indicators(self, *largs):
        if self.ui_root.serial:
            inbound, outbound = self.ui_root.serial.pop_recent_traffic()
            self.ui_root.indicators.update(inbound, outbound)

get = None
