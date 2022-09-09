from pyroborobo import Pyroborobo, WorldObserver
from math import dist
import custom.objects as CObj
import numpy as np
import random

#Custom world objserver class
class NSLCWorldObserver(WorldObserver):
    
    #World observer parameters
    NB_ROBOTS = 100
    NB_OBJECTS_SMALL = 50
    NB_OBJECTS_MEDIUM = 20
    NB_OBJECTS_LARGE = 5
    NB_OBJECTS = NB_OBJECTS_SMALL + NB_OBJECTS_MEDIUM + NB_OBJECTS_LARGE
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

    #Post world initialization
    def init_post(self):
        super().init_post()

        #Add robots
        for i in range(NSLCWorldObserver.NB_ROBOTS):
            self.rob.add_robot()

        #Randomly generate robot locations outside gathering zone
        for robot in self.rob.controllers:
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos[0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            robot.set_absolute_orientation(random.randint(-180,180))
            robot.set_position(pos[0], pos[1])

        #Add small resources at random locations outside gathering zone
        for i in range(NSLCWorldObserver.NB_OBJECTS_SMALL):
            obj = CObj.SmallObject(i, {})
            obj = self.rob.add_object(obj)
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos[0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            obj.unregister()
            obj.set_coordinates(pos[0], pos[1])
            obj.register()
            obj.show()

        #Add large resources at random locations outside gathering zone
        for i in range(NSLCWorldObserver.NB_OBJECTS_MEDIUM):
            obj = CObj.MediumObject(i + NSLCWorldObserver.NB_OBJECTS_SMALL, {})
            obj = self.rob.add_object(obj)
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos[0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            obj.unregister()
            obj.set_coordinates(pos[0], pos[1])
            obj.register()
            obj.show()

        #Add large resources at random locations outside gathering zone
        for i in range(NSLCWorldObserver.NB_OBJECTS_LARGE):
            obj = CObj.LargeObject(i + NSLCWorldObserver.NB_OBJECTS_SMALL + NSLCWorldObserver.NB_OBJECTS_MEDIUM, {})
            obj = self.rob.add_object(obj)
            pos = NSLCWorldObserver.generate_random_position()
            while pos[0] >= NSLCWorldObserver.COLLECTION_START[0] and pos[0] <= NSLCWorldObserver.COLLECTION_END[0] and pos[1] >= NSLCWorldObserver.COLLECTION_START[1] and pos[1] <= NSLCWorldObserver.COLLECTION_END[1]:
                pos = NSLCWorldObserver.generate_random_position()
            obj.unregister()
            obj.set_coordinates(pos[0], pos[1])
            obj.register()
            obj.show()

    #Post iteration
    def step_post(self):
        super().step_post()
        #If all resources placed, set end iteration
        if self.objects_placed == NSLCWorldObserver.NB_OBJECTS and not self.finished:
            self.end_it = self.iteration
            self.finished = True
        self.iteration += 1

    #Pre iteration
    def step_pre(self):
        super().step_pre()
        self.next_gen_in_it -= 1
        #If new generation, get average distance of swarm to gathering zone for generation
        if self.next_gen_in_it < 0:
            avg_dist = round((self.distance_to_gather/self.next_gen_every_it),2)
            self.distances_to_gather.append(avg_dist)
            self.distance_to_gather = 0
            self.next_gen_in_it = self.next_gen_every_it

        #Get average x and y positions for all robots
        x_swarm = 0
        y_swarm = 0
        for c in self.rob.controllers:
            pos = c.absolute_position
            x_swarm += pos[0]
            y_swarm += pos[1]
        
        x_swarm = x_swarm/NSLCWorldObserver.NB_ROBOTS
        y_swarm = y_swarm/NSLCWorldObserver.NB_ROBOTS

        #Increase distance to gathering zone for swarm
        swarm_pos = (x_swarm, y_swarm)
        self.distance_to_gather += dist(swarm_pos, NSLCWorldObserver.COLLECTION_CENTER)

    #Generate a random position within the arena
    def generate_random_position():
        x = np.random.randint(13, NSLCWorldObserver.ARENA_WIDTH-20)
        y = np.random.randint(17, NSLCWorldObserver.ARENA_HEIGHT-25)
        position = (x, y)
        return position

    #Increment objects placed
    def increment_objects_placed(self):
        self.objects_placed += 1

    #Decrement objects placed
    def decrement_objects_placed(self):
        self.objects_placed -= 1
