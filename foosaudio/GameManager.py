from SoundBank import SoundBank

__author__ = 'andrewwang'

import serial
import time
import pygame
from multiprocessing import Process, Queue
from collections import deque
import atexit

GOAL_SOUND_PATH = "./sounds/"
ARDUINO_SERIAL_ADDR = "/dev/ttyACM0"
IDLE_THRESHOLD_SEC = 300
IDLE_SOUND_THRESHOLD_SEC = 30
MULTI_GOAL_STREAK = 3

class GameManager():
    def __init__(self, sounds_dir):
        self.arduino = None
        self.serial_checker = None
        self.goal_q = Queue(1)
        self.motion_q = Queue(1)

        self.sounds = SoundBank(sounds_dir)

        self.previous_goals = deque(maxlen=10)
        self.previous_goal_times = deque(maxlen=10)

        self.previous_motion = deque(maxlen=100)
        self.previous_motion_times = deque(maxlen=100)
        self.start_of_game = 0
        self.last_event_time = 0
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

    def check_serial(self, goal_q, motion_q):
        while True:
            ascii_val = self.arduino.readline()
            if "A" in ascii_val:
                 goal_q.put("A")
            elif "B" in ascii_val:
                goal_q.put("B")
            elif "S" in ascii_val:
                motion_q.put("S")
            elif "M" in ascii_val:
                motion_q.put("M")
            elif "L" in ascii_val:
                motion_q.put("L")
            else:
                pass

    def run(self):
        self.serial_checker = Process(target=self.check_serial, args=(self.goal_q, self.motion_q))
        self.serial_checker.start()

        while True:
            sound_to_play = self.determine_sound_for_goal()  # High Priority
            if sound_to_play is None:
                sound_to_play = self.determine_sound_for_motion()     # Low Priority

            self.play_sound(sound_to_play)


    def event_occurred(self):
        # record start of game
        self.last_event_time = time.time()
        if time.time() > self.last_event_time + IDLE_THRESHOLD_SEC:
            self.start_of_game = time.time()


    def determine_sound_for_goal(self):
        if self.goal_q.full():
            value = self.goal_q.get()

            if value == "A" or value == "B":
                self.event_occurred()
                self.previous_goal_times.append(time.time())
                self.previous_goals.append(value)
                return self.goal_event_sound()

        return None

    def determine_sound_for_motion(self):
        if self.motion_q.full():
            value = self.motion_q.get()

            if value == "S" or value == "M" or value == "L":
                self.event_occurred()
                self.previous_motion_times.append(time.time())
                self.previous_motion.append(value)
                return self.motion_event_sound()

        return None

    def goal_event_sound(self):

        # first goal of the game
        if len(self.previous_goal_times) <= 1 or \
                        self.previous_goal_times[-1] - self.previous_goal_times[-2] > IDLE_THRESHOLD_SEC:
            return self.sounds.first_event_sound()

        # goal streak
        elif len(self.previous_goals.length) >= MULTI_GOAL_STREAK:
            most_recent_goals = list(reversed(self.previous_goals))[0:MULTI_GOAL_STREAK]
            if most_recent_goals[1:] == most_recent_goals[:-1]:
                return self.sounds.multi_goal_sound()

        # revenge on goal streak
        elif len(self.previous_goals.length) >= MULTI_GOAL_STREAK + 1:
            most_recent_goals = list(reversed(self.previous_goals))[1:MULTI_GOAL_STREAK + 1]
            if self.previous_goals[-1] != most_recent_goals[0] and \
                            most_recent_goals[1:] == most_recent_goals[:-1]:
                 return self.sounds.revenge_goal_sound()

        else:
            return self.sounds.goal_sound()

    def motion_event_sound(self):
        # consistent motion without a goal
        # 10 events after previous goal but last event hasn't exceeded IDLE THRESHOLD
        if time.time() < self.last_event_time + IDLE_THRESHOLD_SEC and \
                        len(self.previous_game_times) > 1 and \
                        time.time() > self.previous_goal_times[-1] + IDLE_SOUND_THRESHOLD_SEC:
            print("motion sound")

        return None



    def play_sound(self, file):
        if file:
            pygame.mixer.init()

            # Stop the mixer if something's playing
            if pygame.mixer.music.get_busy() == True:
                pygame.mixer.stop()

            pygame.mixer.music.load(file)
            pygame.mixer.music.play()

    def cleanup(self):
        try:
            if self.serial_checker:
                self.serial_checker.terminate()

            if self.arduino:
                self.arduino.close()

            pygame.mixer.quit()

        except Exception as e:
            print("Cleanup exception: " + str(e))


if __name__ == '__main__':
    g = GameManager(GOAL_SOUND_PATH)
    g.connect_to_arduino(ARDUINO_SERIAL_ADDR)
    g.run()
