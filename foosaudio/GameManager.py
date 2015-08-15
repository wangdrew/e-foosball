__author__ = 'andrewwang'

import serial
from os import listdir
from os.path import isfile, join
import time
import random
import pygame
from multiprocessing import Process, Queue

MAX_RETRY = 5
GOAL_SOUND_PATH = "./sounds/goal/"
ARDUINO_SERIAL_ADDR = "/dev/ttyUSB0"

class GameManager():
    def __init__(self, sounds_dir):
        self.goal_q = Queue(1)
        self.sounds = sounds_dir
        self.sound_files = [f for f in listdir(sounds_dir) if isfile(join(sounds_dir, f))]

    def connect_to_arduino(self, serial_addr):

        try:
            self.arduino = serial.Serial(serial_addr,
                                         baudrate=9600,
                                         bytesize=serial.EIGHTBITS,
                                         parity=serial.PARITY_NONE,
                                         stopbits=serial.STOPBITS_ONE,
                                         timeout=.1)
        except Exception as e:
            print("error opening serial connection")
            raise e

    def check_goal(self, q):
        ascii_val = self.arduino.readline()
        print(ascii_val)

        if ascii_val is chr(ord('A')):
            q.put("A")
        elif ascii_val is chr(ord('B')):
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
        self.play_goal_sound()

    def goal_b(self):
        self.play_goal_sound()

    def play_goal_sound(self):
        #print str(self.sound_files)
        file = GOAL_SOUND_PATH + self.sound_files[random.randint(0, len(self.sound_files) - 1)]
        #print str(file)
        pygame.mixer.init()
        pygame.mixer.music.load(file)
        pygame.mixer.music.play()
        while pygame.mixer.music.get_busy() == True:
            continue
        time.sleep(1)

if __name__ == '__main__':
    g = GameManager(GOAL_SOUND_PATH)
    g.connect_to_arduino(ARDUINO_SERIAL_ADDR)
    g.run()
