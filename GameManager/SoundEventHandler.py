from os import listdir
from os.path import isfile, join
import time
import random
from EventHandler import EventHandler

from pydub import AudioSegment
from pydub.playback import play

class SoundEventHandler(EventHandler):

    sound_path = "/home/pi/dev/e-foosball/GameManager/sounds"  # TODO: hardcoded for now

    def __init__(self):
        # Mutable state
        self.previous_goal = ""
        self.consecutive_goals = 0
        self.total_goals = 0

        self.consecutive_goal_sound = {
            1: lambda: self.get_sound_from_dir("goal"),
            2: lambda: self.get_sound_from_dir("second_goal") if self.total_goals == 2 else self.get_sound_from_dir("goal"),
            3: lambda: self.get_sound_from_dir("third_goal"),
            4: lambda: self.get_sound_from_dir("fourth_goal"),
            5: lambda: self.get_sound_from_dir("fifth_goal")
        }

        self.number_sound = {
            1: "One.wav",
            2: "Two.wav",
            3: "Three.wav",
            4: "Four.wav",
            5: "Five.wav",
            6: "Six.wav",
            7: "Seven.wav",
            8: "Eight.wav",
            9: "Nine.wav"
        }

    def process_event(self, event):
        sound = None

        # new game event
        if "newgame" in event:
            sound = self.get_sound_byte(self.get_sound_from_dir("new_game"))
            self.previous_goal = ""
            self.consecutive_goals = 0
            self.total_goals = 0

        # goal event
        elif "A" is event[0] or "B" is event[0]:  # A or B is the 1st character
            self.total_goals += 1
            current_goal = event[0]      # extract which goal "A" or "B"
            a_num_goals = int(event[2])  # extract number of A and B goals
            b_num_goals = int(event[4])

            # first goal
            if self.previous_goal == "":
                self.consecutive_goals = 1
                sound = self.get_sound_byte(self.get_sound_from_dir("opener_goal"))

            # goal streak
            elif self.previous_goal == current_goal:
                self.consecutive_goals += 1
                sound = self.get_sound_byte(self.consecutive_goal_sound[self.consecutive_goals]())

            # comeback and all other goals
            else:
                if self.consecutive_goals == 4:
                    sound = self.get_sound_byte(self.get_sound_from_dir("comeback"))
                else:
                    sound = self.get_sound_byte(self.get_sound_from_dir("goal"))
                self.consecutive_goals = 1

            # announce score
            if sound and a_num_goals > 0 and b_num_goals > 0:
                sound += self.get_sound_byte(self.get_sound_from_number_dir(a_num_goals))
                sound += self.get_sound_byte(self.get_sound_from_number_dir(b_num_goals))

            self.previous_goal = current_goal

        # unknown event
        else:
            pass

        if sound:
            play(sound)


    def get_sound_byte(self, sound_path):
        return AudioSegment.from_wav(sound_path)

    def get_sound_from_dir(self, folder_name):
        path = self.sound_path + "/" + folder_name
        file_list = [f for f in listdir(path) if isfile(join(path, f))]
        return path + "/" + file_list[random.randint(0, len(file_list) - 1)]

    def get_sound_from_number_dir(self, num):
        if 0 < num < 10:
            return self.sound_path + "/numbers/" + self.number_sound[num]
        else:
            return None

    def cleanup(self):
        pass

