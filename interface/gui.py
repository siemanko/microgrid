from kivy.app import App
from kivy.properties import (
    ObjectProperty,
    StringProperty,
)
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

import controller

from message_handler import MessageHandler
from serial_adapter import SerialAdapter

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

class MenuView(BoxLayout):
    pass

class DebugView(BoxLayout):
    logs=ObjectProperty(None)

class SettingsView(BoxLayout):
    pass
    # box_time          =   StringProperty("")
    # box_uid           =   StringProperty("")
    # box_node_type     =   StringProperty("")
    # box_balance       =   StringProperty("")

class RootView(BoxLayout):
    serial = ObjectProperty(None)

    def __init__(self, *args, **kwargs):
        super(RootView, self).__init__(*args, **kwargs)

        self.serial = SerialAdapter('/dev/ttyUSB0',
                                    self.on_message)
        self.message_handler = MessageHandler(self)


    def on_message(self, msg):
        self.message_handler.handle(msg)

class MicrogridApp(App):
    def build(self):
        self.root_view = RootView()
        controller.get = controller.Ctrl(self.root_view)
        return self.root_view


