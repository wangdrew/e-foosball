from os import listdir
from os.path import isfile, join
import time
import random
from EventHandler import EventHandler
from SoundOutput import SoundOutput


class SoundEventHandler(EventHandler):

    sound_path = "/home/pi/dev/e-foosball/GameManager/sounds"  # TODO: hardcoded for now

    def __init__(self):
        self.sound = SoundOutput()
        self.sound.run()

        # Mutable state
        self.previous_goal = ""
        self.consecutive_goals = 0
        self.total_goals = 0

        self.play_for_consecutive_goal = {
            1: lambda: self.play_sound_in("goal"),
            2: lambda: self.play_sound_in("second_goal") if self.total_goals == 2 else self.play_sound_in("goal"),
            3: lambda: self.play_sound_in("third_goal"),
            4: lambda: self.play_sound_in("fourth_goal"),
            5: lambda: self.play_sound_in("fifth_goal")
        }

        self.number_sound_files = {
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
        if "newgame" in event:
            self.play_sound_in("new_game")
            self.previous_goal = ""
            self.consecutive_goals = 0
            self.total_goals = 0
        elif "A" is event[0] or "B" is event[0]: # A or B is the 1st character
            self.total_goals += 1
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

            try:
                a_num_goals = int(event[2])
                b_num_goals = int(event[4])
                if a_num_goals + b_num_goals > 2:
                    self.announce_score(a_num_goals, b_num_goals)
            except:
                pass

        else:
            pass    # Ignore unknown event


    def play_sound_in(self, folder_name):
        path = self.sound_path + "/" + folder_name
        file_list = [f for f in listdir(path) if isfile(join(path, f))]
        self.sound.play_sound_now(path + "/" + file_list[random.randint(0, len(file_list) - 1)])

    def announce_score(self, a_num_goals, b_num_goals):
        print("announcing: " + str(a_num_goals) + " " + str(b_num_goals))
        path = self.sound_path + "/numbers/"
        self.sound.queue_sound(path + self.number_sound_files[a_num_goals])
        self.sound.queue_sound(path + self.number_sound_files[b_num_goals])

    def cleanup(self):
        self.sound.cleanup()

