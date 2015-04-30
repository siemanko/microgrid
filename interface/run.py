from kivy.lang import Builder
from os.path import dirname, realpath, join

from gui import MicrogridApp

from kivy.base import EventLoop
from kivy.resources import resource_add_path
SCRIPT_DIR = dirname(realpath(__file__))

resource_add_path(join(SCRIPT_DIR, 'resources'))

Builder.load_file(join(SCRIPT_DIR, 'gui.kv'))

if __name__ == '__main__':
    EventLoop.ensure_window()
    MicrogridApp().run()
