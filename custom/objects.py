from pyroborobo import MovableObject, Pyroborobo, SquareObject
from custom.world_observers import NSLCWorldObserver as NSLCWO
import numpy as np

#Class for small resource
class SmallObject(MovableObject):

    #Parameters for small resource
    ROBOTS_REQ = 1
    OBJECT_SIZE = 6
    FOOTPRINT_SIZE = 10
    COLOUR = (192, 255, 128)
    POINTS = 1

    def __init__(self, id_, data):
        super().__init__(id_)
        self.data = data
        self.placed = False
        self.nb_bound = 0
        self.unregister()
        self.set_radius(type(self).OBJECT_SIZE)
        self.set_footprint_radius(type(self).FOOTPRINT_SIZE)
        red = type(self).COLOUR[0]
        green = type(self).COLOUR[1]
        blue = type(self).COLOUR[2]
        self.set_color(red, green, blue)
        x = np.random.randint(NSLCWO.COLLECTION_START[0], NSLCWO.COLLECTION_END[0])
        y = np.random.randint(NSLCWO.COLLECTION_START[1], NSLCWO.COLLECTION_END[1])
        self.dest = (x, y)
        self.register()

    def step(self):
        super().step()

    #If resource is pushed
    def is_pushed(self, id_, speed):
        super().is_pushed(id_, speed)
        p = self.position
        pyrr = Pyroborobo.get()
        world_obs = pyrr.world_observer
        #If resource is in gathering zone and not already placed, set placed to true and increment objects placed
        if p[0] >= NSLCWO.COLLECTION_START[0] and p[0] <= NSLCWO.COLLECTION_END[0] and p[1] >= NSLCWO.COLLECTION_START[1] and p[1] <= NSLCWO.COLLECTION_END[1]:
            if not self.placed:
                self.placed = True
                world_obs.increment_objects_placed()
        #If resource is not in gathering zone and already placed, set placed to false and decrement objects palced
        else:
            if self.placed:
                self.placed = False
                world_obs.decrement_objects_placed()
    def inspect(self, prefix):
        return f"[INSPECT] Ball #{self.id}\n"

#Medium resource class
class MediumObject(SmallObject):

    #Medium resource parameters
    ROBOTS_REQ = 2
    OBJECT_SIZE = 10
    FOOTPRINT_SIZE = 14
    COLOUR = (245, 174, 10)
    POINTS = 2

    def __init__(self, id_, data):
        super().__init__(id_, data)

#Large resource class
class LargeObject(SmallObject):

    #Large resource parameters
    ROBOTS_REQ = 3
    OBJECT_SIZE = 12
    FOOTPRINT_SIZE = 16
    COLOUR = (170, 10, 245)
    POINTS = 3

    def __init__(self, id_, data):
        super().__init__(id_, data)

