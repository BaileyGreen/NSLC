from abc import ABC, abstractmethod
from math import atan2, degrees

#Base heuristic class
class Heuristic(ABC):

    @abstractmethod
    def __init__(self, robot):
        self.robot = robot

    @abstractmethod
    def step(self):
        pass

    #Move robot to specified position
    def moveTo(self, cur_pos, pos):
        angle = Heuristic.angleBetween(cur_pos, pos)

        self.robot.set_absolute_orientation(angle - 180)
        self.robot.set_translation(1)

    #Get the angle between two points
    def angleBetween(p1, p2):
        angle = atan2(p1[1] - p2[1], p1[0] - p2[0])
        angle = degrees(angle)
        return angle
