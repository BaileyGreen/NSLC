from pyroborobo import Pyroborobo, WorldObserver
from math import dist
import custom.objects as CObj
import numpy as np
import random

class NSLCWorldObserver(WorldObserver):
    
    NB_ROBOTS = 100
    NB_OBJECTS_EASY = 50
    NB_OBJECTS_MEDIUM = 20
    NB_OBJECTS_HARD = 5
    NB_OBJECTS = NB_OBJECTS_EASY + NB_OBJECTS_MEDIUM + NB_OBJECTS_HARD
    COLLECTION_START = (0, 400)
    COLLECTION_END = (400, 800)
    COLLECTION_CENTER = ((COLLECTION_START[0] + COLLECTION_END[0])/2, (COLLECTION_START[1] + COLLECTION_END[1])/2)
    ARENA_WIDTH = 800
    ARENA_HEIGHT = 800

    def __init__(self, world):
        super().__init__(world)
        self.rob = Pyroborobo.get()
        self.objects_placed = 0
        self.iteration = 1
        self.end_it = 10000
        self.next_gen_every_it = 1000
        self.next_gen_in_it = self.next_gen_every_it
        self.finished = False
        self.distances_to_gather = list()
        self.distance_to_gather = 0

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
        if self.objects_placed == NSLCWorldObserver.NB_OBJECTS and not self.finished:
            self.end_it = self.iteration
            self.finished = True
        self.iteration += 1

    def step_pre(self):
        super().step_pre()
        self.next_gen_in_it -= 1
        if self.next_gen_in_it < 0:
            avg_dist = round((self.distance_to_gather/self.next_gen_every_it),2)
            self.distances_to_gather.append(avg_dist)
            self.distance_to_gather = 0
            self.next_gen_in_it = self.next_gen_every_it

        x_swarm = 0
        y_swarm = 0
        for c in self.rob.controllers:
            pos = c.absolute_position
            x_swarm += pos[0]
            y_swarm += pos[1]
        
        x_swarm = x_swarm/NSLCWorldObserver.NB_ROBOTS
        y_swarm = y_swarm/NSLCWorldObserver.NB_ROBOTS

        swarm_pos = (x_swarm, y_swarm)
        self.distance_to_gather += dist(swarm_pos, NSLCWorldObserver.COLLECTION_CENTER)

    def generate_random_position():
        x = np.random.randint(13, NSLCWorldObserver.ARENA_WIDTH-20)
        y = np.random.randint(17, NSLCWorldObserver.ARENA_HEIGHT-25)
        position = (x, y)
        return position

    def increment_objects_placed(self):
        self.objects_placed += 1

    def decrement_objects_placed(self):
        self.objects_placed -= 1
