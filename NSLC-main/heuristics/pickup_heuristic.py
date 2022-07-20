from heuristics.heuristic import Heuristic
from custom.world_observers import NSLCWorldObserver as NSLCWO
from math import radians, cos, sin, dist

class PickupHeuristic(Heuristic):

    def __init__(self, robot, object):
        super().__init__(robot)
        self.object = object
        self.stationary_it = 0
        self.prev_pos = robot.absolute_position

    def step(self):
        if self.object.placed:
            self.robot.heuristic = None
            self.object.nb_bound -= 1
        
        if self.prev_pos == self.robot.absolute_position:
            self.stationary_it += 1
        else:
            self.stationary_it = 0

        if self.stationary_it >= 50:
            self.robot.heuristic = None
            self.robot.wait_it = 100
            self.object.nb_bound -= 1

        distance_to_obj = dist(self.object.position, self.robot.absolute_position)

        if self.object.nb_bound < type(self.object).ROBOTS_REQ and distance_to_obj < type(self.object).OBJECT_SIZE + 2:
            self.robot.set_rotation(0)
            self.robot.set_translation(0)
        else:
            r = type(self.object).OBJECT_SIZE
            angle = Heuristic.angleBetween(self.robot.absolute_position, ((NSLCWO.COLLECTION_START[0]+NSLCWO.COLLECTION_END[0])/2, (NSLCWO.COLLECTION_START[1]+NSLCWO.COLLECTION_END[1])/2))
            angle = radians(angle)
            obj_position = self.object.position
            x = obj_position[0] + r * cos(angle)
            y = obj_position[1] + r * sin(angle)
            pos = (x, y)
            self.moveTo(self.robot.absolute_position, pos)
            self.prev_pos = self.robot.absolute_position