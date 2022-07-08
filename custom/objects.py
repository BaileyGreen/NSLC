from pyroborobo import MovableObject, Pyroborobo, SquareObject
from custom.controllers import NSLCController
from custom.world_observers import NSLCWorldObserver as NSLCWO

class BallObject(MovableObject):

    def __init__(self, id_, data):
        MovableObject.__init__(self, id_)
        self.data = data
        self.placed = False
        self.is_bound = False

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