from heuristics.heuristic import Heuristic
from custom.world_observers import NSLCWorldObserver as NSLCWO
from math import radians, cos, sin, dist
import numpy as np

#Pickup heuristic class
class PickupHeuristic(Heuristic):

    def __init__(self, robot, object):
        super().__init__(robot)
        self.object = object
        self.stationary_it = 0
        self.prev_pos = robot.absolute_position

    def step(self):
        #If small resource, increment time spent pushing small resource
        if type(self.object).POINTS == 1:
            self.robot.time_spent_small += 1
        #If medium resource, increment time spent pushing medium resource
        elif type(self.object).POINTS == 2:
            self.robot.time_spent_medium += 1
        #If large resource, increment time spent pushing large resource
        else:
            self.robot.time_spend_large += 1
        
        #If resource has been stationary, increment staionary counter
        if self.prev_pos == self.robot.absolute_position:
            self.stationary_it += 1
        #Otherwise reset staionary counter
        else:
            self.stationary_it = 0

        #If resource has been stationary for longer than 100 iterations
        if self.stationary_it >= 100:
            #If the resource is placed, increment objects deposited
            if self.object.placed:
                self.robot.objects_deposited += type(self.object).POINTS

            #Remove heuristic and set waiting time for being able to push object again    
            self.robot.heuristic = None
            self.robot.wait_it = 80
            self.object.nb_bound -= 1

        #Get difference between resource position and resource destination
        obj_position = self.object.position
        x_diff = abs(obj_position[0] - self.object.dest[0])
        y_diff = abs(obj_position[1] - self.object.dest[1])

        #If position difference is less than 25, set object to placed and remove heuristic
        if x_diff <= 25 and y_diff <=25 and self.object.placed:
            self.robot.objects_deposited += type(self.object).POINTS
            self.robot.heuristic = None
            self.object.nb_bound -=1

        #Get distance to object
        distance_to_obj = dist(self.object.position, self.robot.absolute_position)

        #If number of robots bound to resource is less than required, and robot within specified distance of object,
        #set translation and rotation to 0
        if self.object.nb_bound < type(self.object).ROBOTS_REQ and distance_to_obj < type(self.object).FOOTPRINT_SIZE + 2:
            self.robot.set_rotation(0)
            self.robot.set_translation(0)
        
        #Otherwise calculate where to push resource
        else:
            r = type(self.object).OBJECT_SIZE
            angle = Heuristic.angleBetween(self.robot.absolute_position, self.object.dest)
            angle = radians(angle)
            x = obj_position[0] + r * cos(angle)
            y = obj_position[1] + r * sin(angle)
            pos = (x, y)
            self.moveTo(self.robot.absolute_position, pos)
            self.prev_pos = self.robot.absolute_position