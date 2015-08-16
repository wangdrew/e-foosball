__author__ = 'andrewwang'

import serial
from os import listdir
from os.path import isfile, join
import time
import random
import pygame
from multiprocessing import Process, Queue
import atexit

MAX_RETRY = 5
GOAL_SOUND_PATH = "./sounds/goal/"
ARDUINO_SERIAL_ADDR = "/dev/ttyACM0"

class GameManager():
    def __init__(self, sounds_dir):
        self.goal_q = Queue(1)
        self.sounds = sounds_dir
        self.sound_files = [f for f in listdir(sounds_dir) if isfile(join(sounds_dir, f))]
        pygame.mixer.init()
        atexit.register(self.cleanup())

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

    def check_goal(self, q):
        while True:
            ascii_val = self.arduino.readline()
            print(ascii_val)
            if "A" in ascii_val:
                q.put("A")
            elif "B" in ascii_val:
                q.put("B")
            else:
                pass

    def run(self):
        goal_checker = Process(target=self.check_goal, args=(self.goal_q,))
        goal_checker.start()

        while True:
            if self.goal_q.full() and self.goal_q.get() == "A":
                self.goal_a()
            elif self.goal_q.full() and self.goal_q.get() == "B":
                self.goal_b()
            else:
                pass

    def goal_a(self):
        print("Goal A Sound")
        self.play_goal_sound()

    def goal_b(self):
        print("Goal B Sound")
        self.play_goal_sound()

    def play_goal_sound(self):
        file = GOAL_SOUND_PATH + self.sound_files[random.randint(0, len(self.sound_files) - 1)]

        # Stop the mixer if something's playing
        if pygame.mixer.music.get_busy() == True:
            pygame.mixer.stop()

        pygame.mixer.music.load(file)
        pygame.mixer.music.play()

    def cleanup(self):
        try:
            pygame.mixer.music.quit()
            self.arduino.close()
        except Exception as e:
            print("Cleanup exception: " + str(e))


if __name__ == '__main__':
    g = GameManager(GOAL_SOUND_PATH)
    g.connect_to_arduino(ARDUINO_SERIAL_ADDR)
    g.run()
