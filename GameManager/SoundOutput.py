from multiprocessing import Process, Lock
import pygame


class SoundOutput():
    def __init__(self):
        pygame.mixer.init()
        self.pygame_thread = None
        self.sound_q = Queue(20)
        self.pygame_lock = Lock()

    def run(self):
        self.pygame_thread = Process(target=self.sound_output_thread, args=(self.sound_q, self.pygame_lock,))
        self.pygame_thread.start()

    def queue_sound(self, sound_file):
        print("Adding sound to Q: " + sound_file)
        self.sound_q.put(sound_file)

    def play_sound_now(self, sound_file):
        self.pygame_lock.acquire()
        print("Playing immediately: " + sound_file)
        if pygame.mixer.music.get_busy():
            pygame.mixer.stop()
        pygame.mixer.music.load(sound_file)
        pygame.mixer.music.play()
        self.pygame_lock.release()

    def sound_output_thread(self, sound_q, pygame_lock):
        while True:
            if not pygame.mixer.music.get_busy() and not sound_q.empty():
                pygame_lock.acquire()
                sound_file = sound_q.get()
                print("Playing queued sound: " + sound_file)
                pygame.mixer.music.load(sound_file)
                pygame.mixer.music.play()
                pygame_lock.release()

    def cleanup(self):
        try:
            if self.pygame_thread:
                self.pygame_thread.terminate()
        except Exception as e:
            print "Exception cleanup sound output thread" + str(e)
