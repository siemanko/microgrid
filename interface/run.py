import pyximport
pyximport.install()

from gui import MicrogridApp

if __name__ == '__main__':
    MicrogridApp().run()
