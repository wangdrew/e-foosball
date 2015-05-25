__author__ = 'andrewwang'

import serial
from os import listdir
from os.path import isfile, join
import time
import random
import pygame

MAX_RETRY = 5
GOAL_SOUND_PATH = "./sounds/goal/"
ARDUINO_SERIAL_ADDR = "/dev/ttyUSB0"

class GameManager():
    def __init__(self, sounds_dir):
        self.sounds = sounds_dir
        self.sound_files = [f for f in listdir(sounds_dir) if isfile(join(sounds_dir, f))]

    def connect_to_arduino(self, serial_addr):
        try:
            self.arduino = serial.Serial(serial_addr, baudrate=9600, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=.1)
        except Exception as e:
            print("error opening serial connection")
            raise e

    def run(self):
        while True:
            try:
                blob = self.arduino.read(100)
                #self.arduino.flushInput()

                if '0' in blob:
                    self.goal_a()
                elif '1' in blob:
                    self.goal_b()
                else:
                    pass

            except Exception as e:
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

if __name__ == '__main__':
    g = GameManager(GOAL_SOUND_PATH)
    g.connect_to_arduino(ARDUINO_SERIAL_ADDR)
    g.run()


def retry_on_failure(func, *args):
    ret = None
    retry_count = 0

    while True:
        try:
            ret = func(args)
            break
        except Exception as e:
            retry_count += 1
            print "failed, retrying..."
            if retry_count > MAX_RETRY:
                print "Max retries exceeded"
                raise e

            time.sleep(1)

    return ret
