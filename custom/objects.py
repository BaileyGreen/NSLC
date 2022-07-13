from pyroborobo import MovableObject, Pyroborobo, SquareObject
from custom.controllers import NSLCController
from custom.world_observers import NSLCWorldObserver as NSLCWO

class EasyObject(MovableObject):

    ROBOTS_REQ = 1
    OBJECT_SIZE = 6
    FOOTPRINT_SIZE = 10
    COLOUR = (192, 255, 128)

    def __init__(self, id_, data):
        MovableObject.__init__(self, id_)
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
        self.register()

    def step(self):
        super().step()

    def is_pushed(self, id_, speed):
        super().is_pushed(id_, speed)
        p = self.position
        if p[0] >= NSLCWO.COLLECTION_START[0] and p[0] <= NSLCWO.COLLECTION_END[0] and p[1] >= NSLCWO.COLLECTION_START[1] and p[1] <= NSLCWO.COLLECTION_END[1]:
            if not self.placed:
                self.set_color(255,0,0)
                pyrr = Pyroborobo.get()
                index_offset = pyrr.robot_index_offset
                controllers = pyrr.controllers
                world_obs = pyrr.world_observer
                
                for c in controllers:
                    if (c.id + index_offset) == id_:
                        c.increment_objects_deposited()
                self.placed = True
                world_obs.increment_objects_placed()

    def inspect(self, prefix):
        return f"[INSPECT] Ball #{self.id}\n"

class MediumObject(EasyObject):

    ROBOTS_REQ = 2
    OBJECT_SIZE = 8
    FOOTPRINT_SIZE = 12
    COLOUR = (245, 174, 10)

    def __init__(self, id_, data):
        super().__init__(id_, data)

class HardObject(EasyObject):

    ROBOTS_REQ = 3
    OBJECT_SIZE = 10
    FOOTPRINT_SIZE = 14
    COLOUR = (170, 10, 245)

    def __init__(self, id_, data):
        super().__init__(id_, data)

