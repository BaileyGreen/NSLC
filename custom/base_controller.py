from pyroborobo import Controller, Pyroborobo
from archive_item import ArchiveItem
from itertools import chain
from math import atan2, degrees, sqrt
from heuristics.pickup_heuristic import PickupHeuristic
from heuristics.collision_avoidance_heuristic import CAHeuristic
from neuralnetwork.neuralnetwork import NeuralNetwork
import numpy as np

#Base controller class
class BaseController(Controller):

    def __init__(self, wm):
        super().__init__(wm)
        self.weights = None
        self.archive = list()
        self.rob = Pyroborobo.get()
        self.nn = NeuralNetwork([], self.nb_inputs(), 2, 8, 2, 1)
        self.next_gen_every_it = 1000
        self.deactivated = False
        self.next_gen_in_it = self.next_gen_every_it
        self.objects_deposited = 0
        self.heuristic = None
        self.wait_it = 0
        self.distance_travelled = 0
        self.time_spent_small = 0
        self.time_spent_medium = 0
        self.time_spend_large = 0
        self.diversity = []
        self.prev_pos = self.absolute_position

    #Reset the weights and apply the weights to the neural network and add to archive
    def reset(self):
        if self.weights is None:
            self.weights = np.random.uniform(-1, 1, self.nn.get_nb_weights())
            self.nn.set_weights(self.weights)
            archive_item = ArchiveItem(np.array([0, 0]), self.weights.copy(), 0, 0)
            self.archive.append(archive_item)

    def step(self):
        #Decrement counter to next generation
        self.next_gen_in_it -= 1
        #Start new generation and reset counter
        if self.next_gen_in_it < 0:
            self.new_generation()
            self.next_gen_in_it = self.next_gen_every_it

        #If deactivated, disable movement and change color
        if self.deactivated:
            self.set_color(0, 0, 0)
            self.set_translation(0)
            self.set_rotation(0)
        #If not deactivated
        else:
            self.set_color(0, 0, 255)
            # Get inputs from sensors
            i = self.get_inputs()
            inputs = i[0]
            objects_d = i[1]

            #Get closest object index
            min_index = np.argmin(objects_d)

            #If there is no current heuristic
            if self.heuristic is None:
                #If the closest object is closer than 0.5
                if objects_d[min_index] < 0.5:
                    #Get the object, object position and robot postion
                    obj = self.get_object_instance_at(min_index)
                    obj_position = obj.position
                    rob_position = self.absolute_position

                    #If the object has less than the required robots attached, is not already placed and the robot does not need to wait
                    #apply the pickup heuristic for the object
                    if obj.nb_bound < type(obj).ROBOTS_REQ and not obj.placed and self.wait_it == 0:
                        obj.nb_bound += 1
                        pickupHeuristic = PickupHeuristic(self, obj)
                        self.heuristic = pickupHeuristic

                    #Otherwise apply the colision avoidance heuristic
                    else:
                        caHeuristic = CAHeuristic(self, obj)
                        self.heuristic = caHeuristic
                #If the closest object is further than 0.5
                else:
                    #Apply the inputs and weights to the neural network, get the outputs and apply that to the translation and rotation
                    #of the robot 
                    self.nn.set_inputs(inputs)
                    self.nn.set_weights(self.weights)
                    self.nn.step()
                    out = self.nn.get_outputs()
                    trans_speed, rot_speed = out
                    self.set_translation(trans_speed)
                    self.set_rotation(rot_speed)
                
                #If the robot has to wait, decrement the wait counter
                if self.wait_it > 0:
                    self.wait_it -= 1
            #If there is an active heurisitc, step using the heuristic
            else:
                self.heuristic.step()

            #Increase the distance travelled for this iteration
            cur_pos = self.absolute_position
            self.distance_travelled += sqrt((cur_pos[0] - self.prev_pos[0])**2 + (cur_pos[1] - self.prev_pos[1])**2)
            self.prev_pos = cur_pos

        
    #Get the number of inputs from the sensors
    def nb_inputs(self):
        return (self.nb_sensors * 4)

    #Get all inputs from sensors
    def get_inputs(self):
        #Distances from sensors
        dists = self.get_all_distances()
        #If object is robot for each sensor
        is_robots = self.get_all_robot_ids() != -1
        #If objcet is wall for each sensor
        is_walls = self.get_all_walls()
        #If object is resource for each sensor
        is_objects = self.get_all_objects() != -1

        #Get distance to nearest resources
        objects_dist = np.where(is_objects, dists, 1)

        #return inputs
        inputs = np.concatenate([dists, is_robots, is_objects, is_walls])
        assert(len(inputs) == self.nb_inputs())
        return inputs, objects_dist

    #Inspect controller
    def inspect(self, prefix=""):
        output = "inputs: \n" + str(self.get_inputs()) + "\n\n"
        output += "archive: \n"
        output += str(len(self.archive))
        return output

    #Increment objects deposited
    def increment_objects_deposited(self):
        self.objects_deposited += 1