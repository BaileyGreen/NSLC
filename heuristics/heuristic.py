from abc import ABC, abstractmethod
from math import atan2, degrees

class Heuristic(ABC):

    @abstractmethod
    def __init__(self, robot):
        self.robot = robot

    @abstractmethod
    def step(self):
        pass

    def moveTo(self, cur_pos, pos):
        angle = atan2(cur_pos[1] - pos[1], cur_pos[0] - pos[0])
        angle = degrees(angle)

        self.robot.set_absolute_orientation(angle - 180)
        self.robot.set_translation(1)

    def angleBetween(p1, p2):
        angle = atan2(p1[1] - p2[1], p1[0] - p2[0])
        angle = degrees(angle)
        return angle
