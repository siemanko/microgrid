from kivy.app import App
from kivy.uix.button import Button
from kivy.uix.popup import Popup
from kivy.uix.label import Label
from kivy.uix.listview import ListView
from kivy.adapters.simplelistadapter import SimpleListAdapter

from serial_adapter import SerialAdapter

from communication import what_wrapper

popup = Popup(title='Test popup',
              content=Label(text='Hello world'),
              size_hint=(None, None), size=(400, 400))

class LogsView(ListView):
    def __init__(self):
        self.adapter = SimpleListAdapter(
            data=[],
            cls=Label)
        super(LogsView, self).__init__(adapter=self.adapter)

    def push(self, log):
        self.adapter.data.append(log)

    def clear(self):
        self.adapter.data = []

class MicrogridApp(App):
    def build(self):
        self.serial = SerialAdapter('/dev/ttyUSB0', self.on_char)
        self.logs_view = LogsView()

        return self.logs_view

    def on_char(self, c):
        self.logs_view.push(str(c))


