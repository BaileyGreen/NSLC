from pyroborobo import MovableObject, Pyroborobo, SquareObject
from custom.controllers import NSLCController

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
        if p[0] <= 100 and p[1] >= 300:
            if not self.placed:
                print(f"I'm kicked by {id_} with position {p}")
                pyrr = Pyroborobo.get()
                index_offset = pyrr.robot_index_offset
                controllers = pyrr.controllers
                
                for c in controllers:
                    if (c.id + index_offset) == id_:
                        c.increment_objects_deposited()
                self.placed = True

    def inspect(self, prefix):
        return f"[INSPECT] Ball #{self.id}\n"