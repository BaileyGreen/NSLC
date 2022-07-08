from pyroborobo import Pyroborobo, WorldObserver
import custom.objects as CObj
import numpy as np
import random

class NSLCWorldObserver(WorldObserver):
    
    NB_ROBOTS = 20
    NB_OBJECTS = 15
    COLLECTION_START = (0, 300)
    COLLECTION_END = (100, 400)

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
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos [0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            robot.set_absolute_orientation(random.randint(-180,180))
            robot.set_position(pos[0], pos[1])

        for i in range(NSLCWorldObserver.NB_OBJECTS):
            obj = CObj.BallObject(i, {})
            obj = self.rob.add_object(obj)
            obj.set_footprint_radius = 10
            obj.set_radius = 6
            obj.set_color(192, 255, 128)
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos [0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
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
        r = np.random.randint(0, 400, size=2)
        position = tuple(r)
        return position

    def increment_objects_placed(self):
        self.objects_placed += 1
