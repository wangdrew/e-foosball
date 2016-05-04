from os import listdir
from os.path import isfile, join
import time
import random
import pygame
import EventHandler

class SoundEventHandler(EventHandler):

    play_for_consecutive_goal = {
        1: lambda: play_sound_in("goal"),
        2: lambda: play_sound_in("second_goal"),
        3: lambda: play_sound_in("third_goal"),
        4: lambda: play_sound_in("fourth_goal"),
        5: lambda: play_sound_in("fifth_goal")
    }

    def __init__(self):
        self.sound_path = "./sounds/goal"   # TODO: hardcoded for now

        # Mutable state
        self.previous_goal = ""
        self.consecutive_goals = 0

    def process_event(self, event):

        if event == "newgame":
            self.play_sound_in("new_game")
            self.previous_goal = ""
            self.consecutive_goals = 0
        elif "A" is event[2] or "B" is event[2]: # A or B is the 3rd character
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
        play(file_list[random.randint(0, len(file_list) - 1)])

    def play(self, file):
        pygame.mixer.init()
        # Stop the mixer if something's playing
        if pygame.mixer.music.get_busy() == True:
            pygame.mixer.stop()
        pygame.mixer.music.load(file)
        pygame.mixer.music.play()

    def cleanup(self):
        pygame.mixer.quit()

