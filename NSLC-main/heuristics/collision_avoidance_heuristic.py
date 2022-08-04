from heuristics.heuristic import Heuristic

class CAHeuristic(Heuristic):
    
    def __init__(self, robot, object):
        super().__init__(robot)
        self.object = object
        self.prev_pos = robot.absolute_position
        self.rot_speed = 0.5

    def step(self):
        self.robot.set_translation(1)  # Let's go forward
        self.robot.set_rotation(0)
        # Now, our world_model object is a PyWorldModel, we can have access to camera_* properties
        camera_dist = self.robot.get_all_distances()
        camera_rob_id = self.robot.get_all_robot_ids()
        if camera_dist[1] < 0.5 and camera_rob_id[1] == -1:  # if we see something on our right
            self.robot.set_rotation(self.rot_speed)  # turn left
        elif camera_dist[2] < 0.5 and camera_rob_id[2] == -1:  # or in front of us
            self.robot.set_rotation(self.rot_speed) #avoid it
        elif camera_dist[3] < 0.5 and camera_rob_id[3] == -1:  # Otherwise, if we see something on our left
            self.robot.set_rotation(-self.rot_speed)  # turn right
        else:
            self.robot.heuristic = None