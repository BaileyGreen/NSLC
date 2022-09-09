from custom.base_controller import BaseController
from archive_item import ArchiveItem
from itertools import chain
from math import atan2, degrees, sqrt
from heuristics.pickup_heuristic import PickupHeuristic
from heuristics.collision_avoidance_heuristic import CAHeuristic
from neuralnetwork.neuralnetwork import NeuralNetwork
import numpy as np

#mEDEA controller class
class mEDEAController(BaseController):

    def __init__(self, wm):
        super().__init__(wm)
        self.received_weights = dict()

    def step(self):
        super().step()
        #Share weights
        self.share_weights()

    #Get the number of inputs from the sensors
    def nb_inputs(self):
        return (self.nb_sensors * 4 # cam inputs
                )

    #Share weights to all robots in proximity
    def share_weights(self):
        for robot_controller in self.get_all_robot_controllers():
            if robot_controller:
                robot_controller.receive_weights(self.id, self.weights)

    #Receive weights from other robots adding to recieved weights list
    def receive_weights(self, rid, weights):
        self.received_weights[rid] = (weights.copy())

    #New generation
    def new_generation(self):
        #Novelty metric
        nm = np.array([self.objects_deposited, self.distance_travelled])
        #Fitness
        fitness = self.objects_deposited
        #Create archive item and append to archive
        newItem = ArchiveItem(nm, self.weights, None, fitness)
        self.archive.append(newItem)

        #Diversity metrics
        d = [self.time_spent_small, self.time_spent_medium, self.time_spend_large]
        self.diversity.append(d)

        #Reset time spent pushing resources
        self.time_spent_small = 0
        self.time_spent_medium = 0
        self.time_spend_large = 0
        
        #If received weights from other robots
        if self.received_weights:
            #Randomly select new weights
            new_weights_key = np.random.choice(list(self.received_weights.keys()))
            new_weights = self.received_weights[new_weights_key]
            # mutation
            new_weights = np.random.normal(new_weights, 0.1)
            #Reset metrics and clear weights
            self.objects_deposited = 0
            self.distance_travelled = 0
            self.weights = new_weights
            self.received_weights.clear()
            self.deactivated = False
        #If haven't received weights from other robots, deactivate robot for next generation
        else:
            self.deactivated = True