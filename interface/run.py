
from kivy.app import App
from kivy.lang import Builder
from kivy.properties import (
    ObjectProperty,
)
from kivy.uix.button import Button
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.stacklayout import StackLayout
from kivy.uix.popup import Popup
from kivy.uix.label import Label
from kivy.uix.listview import ListView
from kivy.uix.widget import Widget
from kivy.adapters.simplelistadapter import SimpleListAdapter
from os.path import dirname, realpath, join

import controller

from message_handler import MessageHandler
from serial_adapter import SerialAdapter

SCRIPT_DIR = dirname(realpath(__file__))
Builder.load_file(join(SCRIPT_DIR, 'gui.kv'))

class LogsView(BoxLayout):
    pass

class MenuView(BoxLayout):
    pass

class RootView(BoxLayout):
    logs=ObjectProperty(None)

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

if __name__ == '__main__':
    MicrogridApp().run()
