from heuristics.heuristic import Heuristic

#Colision avoidance heuristic
class CAHeuristic(Heuristic):
    
    def __init__(self, robot, object):
        super().__init__(robot)
        self.object = object
        self.prev_pos = robot.absolute_position
        self.rot_speed = 0.5

    def step(self):
        #Go forward
        self.robot.set_translation(1)
        self.robot.set_rotation(0)
        # Get all distances 
        dists = self.robot.get_all_distances()
        #if we see something on our right, turn left
        if dists[1] < 0.25:  
            self.robot.set_rotation(self.rot_speed)  
        #If we see something infront of us, turn left
        elif dists[2] < 0.25:
            self.robot.set_rotation(self.rot_speed)
        #If we see something on out left, turn right
        elif dists[3] < 0.25:
            self.robot.set_rotation(-self.rot_speed)
        #If no potential colision, remove heuristic
        else:
            self.robot.heuristic = None