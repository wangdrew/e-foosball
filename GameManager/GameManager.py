# pip requirements: pygame

__author__ = 'andrewwang'

import serial
from multiprocessing import Process, Queue
import atexit
from EventHandler import EventHandler
from SoundEventHandler import SoundEventHandler

class GameManager():
    event_handlers = []

    def __init__(self):
        self.arduino = None
        self.event_thread = None
        self.event_q = Queue(1)
        atexit.register(self.cleanup())
        self.register_event_handlers()

    def register_event_handlers(self):
        self.event_handlers = [SoundEventHandler()]  # only one handler for now

    def connect_to_arduino(self, serial_addr):
        try:
            self.arduino = serial.Serial(serial_addr,
                                         baudrate=9600,
                                         bytesize=serial.EIGHTBITS,
                                         parity=serial.PARITY_NONE,
                                         stopbits=serial.STOPBITS_ONE)

        except Exception as e:
            print("error opening serial connection")
            raise e

    def poll_serial(self, q):
        while True:
            ascii_line = self.arduino.readline()
            if "e:" in ascii_line:
                q.put(ascii_line[2:])

        
    def run(self):
        self.event_thread = Process(target=self.poll_serial, args=(self.event_q,))
        self.event_thread.start()

        while True:
            if self.event_q.full():
                event = self.event_q.get()
                for h in self.event_handlers:
                    h.process_event(event)


    def cleanup(self):
        try:
            if self.event_thread:
                self.event_thread.terminate()

            if self.arduino:
                self.arduino.close()

            for h in self.event_handlers:
                h.cleanup()

        except Exception as e:
            print("Cleanup exception: " + str(e))


if __name__ == '__main__':
    g = GameManager()
    g.connect_to_arduino("/dev/ttyACM0")
    g.run()
