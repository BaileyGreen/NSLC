from heuristics.heuristic import Heuristic

class CAHeuristic(Heuristic):
    
    def __init__(self, robot, object):
        super().__init__(robot)
        self.object = object
        self.prev_pos = robot.absolute_position