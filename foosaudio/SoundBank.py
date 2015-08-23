from genericpath import isfile
from os import listdir
from os.path import join
import random

__author__ = 'andrewwang'


class SoundBank():
    def __init__(self, sound_path):
        self.root_path = sound_path
        self.first_event_path = self.root_path + "first_event/"
        self.goal_path = self.root_path + "goal/"
        self.multi_goal_path = self.root_path + "multi_goal/"
        self.revenge_goal_path = self.root_path + "revenge_goal/"

        self.first_event_sounds = [f for f in listdir(self.first_event_path) if isfile(join(self.first_event_path, f))]
        self.goal_sounds = [f for f in listdir(self.goal_path) if isfile(join(self.goal_path, f))]
        self.multi_goal_sounds = [f for f in listdir(self.multi_goal_path) if isfile(join(self.multi_goal_path, f))]
        self.revenge_goal_sounds = [f for f in listdir(self.revenge_goal_path) if isfile(join(self.revenge_goal_path, f))]


    def _random_sound_file(self, sound_path, sound_list):
        return sound_path + sound_list[random.randint(0, len(sound_list) - 1)]

    def first_event_sound(self):
        return self._random_sound_file(self.first_event_path, self.first_event_sounds)

    def goal_sound(self):
        return self._random_sound_file(self.goal_path, self.goal_sounds)

    def multi_goal_sound(self):
        return self._random_sound_file(self.multi_goal_path, self.multi_goal_sounds)

    def revenge_goal_sound(self):
        return self._random_sound_file(self.revenge_goal_path, self.revenge_goal_sounds)