import glob

from kivy.app import App
from kivy.properties import (
    ObjectProperty,
    StringProperty,
)
from serial.tools.list_ports import comports
from kivy.uix.button import Button
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.stacklayout import StackLayout
from kivy.uix.popup import Popup
from kivy.uix.label import Label
from kivy.uix.listview import ListView
from kivy.uix.widget import Widget
from kivy.uix.tabbedpanel import TabbedPanel
from kivy.uix.textinput import TextInput
from kivy.adapters.simplelistadapter import SimpleListAdapter
from kivy.lang import Builder

import controller

from message_handler import MessageHandler
from serial_adapter import SerialAdapter

def sets_equal(a,b):
    return len(a.intersection(b)) == len(a.union(b)) == len(a) == len(b)

class SettingsText(TextInput):
    def __init__(self, *args, **kwargs):
        super(SettingsText, self).__init__(*args, **kwargs)
        self.focues = False

    def on_focus(self, instance, value):
        self.focused = value

class SaneLabel(Label):
    pass

class LogEntry(SaneLabel):
    pass

class IndicatorView(BoxLayout):
    inbound_traffic_text = StringProperty('IN:')
    outbound_traffic_text = StringProperty('OUT:')

    def update(self, inbound, outbound):
        in_str = str(inbound or '')
        out_str = str(outbound or '')
        self.inbound_traffic_text = 'IN: ' + in_str
        self.outbound_traffic_text = 'OUT: ' + out_str

class LogsView(BoxLayout):
    pass

class DebugView(BoxLayout):
    logs=ObjectProperty(None)

class SettingsView(BoxLayout):
    pass

class DataLoggerView(BoxLayout):
    pass

class RootView(BoxLayout):
    def __init__(self, *args, **kwargs):

        super(RootView, self).__init__(*args, **kwargs)

        self.message_handler = MessageHandler(self)

        self.serial = None

        self.last_serial_choices = set()


        self.update_serial_choices()

    def on_message(self, msg):
        self.message_handler.handle(msg)

    def on_serial_choice(self, checkbox, value):
        if self.serial is not None:
            if self.serial.port == checkbox.port:
                return
            self.serial.close()
            self.serial = None
        if value:
            self.serial = SerialAdapter(checkbox.port,
                    self.on_message)


    def update_serial_choices(self, *largs):
        extra_mac_ports = set(glob.glob('/dev/tty.usbserial*'))
        new_serial_choices = set([port for port, _, _ in comports()])
        for i_hate_macs in extra_mac_ports:
            new_serial_choices.add(i_hate_macs)

        if not sets_equal(self.last_serial_choices, new_serial_choices):
            self.indicators.serial_choices.clear_widgets()
            self.last_serial_choices = new_serial_choices
            for port in sorted(list(new_serial_choices)):
                port_name = port
                if port_name.startswith('/dev/'):
                    port_name = port[5:]
                btn = Builder.load_string('''
CheckBox:
    size_hint_y: 1
    group: 'serial_choice_group'
                ''')
                lbl = Builder.load_string('''
SaneLabel:
    size_hint_y: 1
    text: '%s'
                ''' % (port_name,))
                btn.bind(active=self.on_serial_choice)
                btn.port = port
                if self.serial is not None and self.serial.port == port:
                    btn.active = True
                self.indicators.serial_choices.add_widget(btn)
                self.indicators.serial_choices.add_widget(lbl)



class MicrogridApp(App):
    def build(self):

        self.root_view = RootView()
        controller.get = controller.Ctrl(self.root_view)
        return self.root_view


