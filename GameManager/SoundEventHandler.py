from os import listdir
from os.path import isfile, join
import time
import random
import pygame
from EventHandler import EventHandler

class SoundEventHandler(EventHandler):

    sound_path = "./sounds"  # TODO: hardcoded for now

    def __init__(self):
        # Mutable state
        self.previous_goal = ""
        self.consecutive_goals = 0

        self.play_for_consecutive_goal = {
            1: lambda: self.play_sound_in("goal"),
            2: lambda: self.play_sound_in("second_goal"),
            3: lambda: self.play_sound_in("third_goal"),
            4: lambda: self.play_sound_in("fourth_goal"),
            5: lambda: self.play_sound_in("fifth_goal")
        }

    def process_event(self, event):
        if "newgame" in event:
            self.play_sound_in("new_game")
            self.previous_goal = ""
            self.consecutive_goals = 0
        elif "A" is event[0] or "B" is event[0]: # A or B is the 1st character
            current_goal = event[0]     # extract which goal "A" or "B"
            if self.previous_goal == "":
                self.consecutive_goals = 1
                self.play_sound_in("opener_goal")

            elif self.previous_goal == current_goal:
                self.consecutive_goals += 1
                self.play_for_consecutive_goal[self.consecutive_goals]()

            else:
                if self.consecutive_goals == 4:
                    self.play_sound_in("comeback")
                else:
                    self.play_sound_in("goal")
                self.consecutive_goals = 1

            self.previous_goal = current_goal

        else:
            pass    # Ignore unknown event


    def play_sound_in(self, folder_name):
        path = self.sound_path + "/" + folder_name
        file_list = [f for f in listdir(path) if isfile(join(path, f))]
        self.play(file_list[random.randint(0, len(file_list) - 1)])

    def play(self, file):
        pygame.mixer.init()
        # Stop the mixer if something's playing
        if pygame.mixer.music.get_busy():
            pygame.mixer.stop()
        pygame.mixer.music.load(file)
        pygame.mixer.music.play()

    def cleanup(self):
        pass
        pygame.mixer.quit()

