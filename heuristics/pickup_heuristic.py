from heuristics.heuristic import Heuristic
from math import radians, cos, sin

class PickupHeuristic(Heuristic):

    def __init__(self, robot, object):
        super().__init__(robot)
        self.object = object
        self.stationary_it = 0
        self.prev_pos = robot.absolute_position

    def step(self):
        if self.prev_pos == self.robot.absolute_position:
            self.stationary_it += 1
        else:
            self.stationary_it = 0

        if self.stationary_it >= 50:
            self.robot.heuristic = None
            self.robot.wait_it = 100
            self.object.is_bound = False

        r = 6
        angle = Heuristic.angleBetween(self.robot.absolute_position, (50, 350))
        angle = radians(angle)
        obj_position = self.object.position
        x = obj_position[0] + r * cos(angle)
        y = obj_position[1] + r * sin(angle)
        pos = (x, y)
        self.moveTo(self.robot.absolute_position, pos)
        self.prev_pos = self.robot.absolute_position