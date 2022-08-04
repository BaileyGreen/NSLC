from pyroborobo import Pyroborobo, WorldObserver
import custom.objects as CObj
import numpy as np
import random

class NSLCWorldObserver(WorldObserver):
    
    NB_ROBOTS = 100
    NB_OBJECTS_EASY = 50
    NB_OBJECTS_MEDIUM = 15
    NB_OBJECTS_HARD = 5
    COLLECTION_START = (0, 400)
    COLLECTION_END = (400, 800)
    ARENA_WIDTH = 800
    ARENA_HEIGHT = 800

    def __init__(self, world):
        super().__init__(world)
        self.rob = Pyroborobo.get()
        self.objects_placed = 0

    def init_pre(self):
        super().init_pre()

    def init_post(self):
        super().init_post()

        for i in range(NSLCWorldObserver.NB_ROBOTS):
            self.rob.add_robot()

        for robot in self.rob.controllers:
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos[0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            robot.set_absolute_orientation(random.randint(-180,180))
            robot.set_position(pos[0], pos[1])

        for i in range(NSLCWorldObserver.NB_OBJECTS_EASY):
            obj = CObj.EasyObject(i, {})
            obj = self.rob.add_object(obj)
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos[0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            obj.unregister()
            obj.set_coordinates(pos[0], pos[1])
            obj.register()
            obj.show()

        for i in range(NSLCWorldObserver.NB_OBJECTS_MEDIUM):
            obj = CObj.MediumObject(i + NSLCWorldObserver.NB_OBJECTS_EASY, {})
            obj = self.rob.add_object(obj)
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos[0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            obj.unregister()
            obj.set_coordinates(pos[0], pos[1])
            obj.register()
            obj.show()

        for i in range(NSLCWorldObserver.NB_OBJECTS_HARD):
            obj = CObj.HardObject(i + NSLCWorldObserver.NB_OBJECTS_EASY + NSLCWorldObserver.NB_OBJECTS_MEDIUM, {})
            obj = self.rob.add_object(obj)
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos[0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            obj.unregister()
            obj.set_coordinates(pos[0], pos[1])
            obj.register()
            obj.show()

    def step_post(self):
        super().step_post()

    def step_pre(self):
        super().step_pre()

    def generate_random_position():
        x = np.random.randint(0, NSLCWorldObserver.ARENA_WIDTH)
        y = np.random.randint(0, NSLCWorldObserver.ARENA_HEIGHT)
        position = (x, y)
        return position

    def increment_objects_placed(self):
        self.objects_placed += 1

    def decrement_objects_placed(self):
        self.objects_placed -= 1
