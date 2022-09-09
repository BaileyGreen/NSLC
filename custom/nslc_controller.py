from custom.base_controller import BaseController
from archive_item import ArchiveItem
from itertools import chain
from math import atan2, degrees, sqrt
from heuristics.pickup_heuristic import PickupHeuristic
from heuristics.collision_avoidance_heuristic import CAHeuristic
from neuralnetwork.neuralnetwork import NeuralNetwork
import numpy as np

#NSLC Controller class
class NSLCController(BaseController):

    def __init__(self, wm):
        super().__init__(wm)
        self.received_archives = list()
        self.archives_received = 0

    def step(self):
        super().step()
        #Share archive
        self.share_archive()

    #Share archive with robots in close proximity
    def share_archive(self):
        for robot_controller in self.get_all_robot_controllers():
            if robot_controller:
                robot_controller.receive_archives(self.archive)
                self.archives_received += 1

    #Receive archives from robots in close proximity and append to list
    def receive_archives(self, archive):
        self.received_archives.append(archive)

    #New generation
    def new_generation(self):
        #Get novelty metric, ftiness and arhive length
        nm = np.array([self.objects_deposited, self.distance_travelled])
        fitness = self.objects_deposited
        archiveLen = len(self.archive)
        
        #k value for number of nearest neighbours to compare
        k = min(200, archiveLen)
        if archiveLen > 0:
            #Get all differences between current novelty metric and novelty metrics in archive
            bdist = list()
            for item in self.archive:
                bdist.append([np.linalg.norm(nm - item.get_novelty_metric()), item.fitness])

            #Sort the differences in novelty metric
            bdist.sort(key=lambda bdist:bdist[0])

            sum = 0
            lcs = 0

            #Loop through the k nearest neighbours
            for i in range (k):
                #Add the differences to the sum
                sum = sum + bdist[i][0]
                #If the fitness value is higher than the neighbour, increment local competition score
                if bdist[i][1] < fitness:
                    lcs += 1
            
            #Get the average distance from the k neighbours
            p = sum/k
            #Calculate the threshold for whether to add current behaviour to the archive using weighted sum
            threshold = 0.5 * p + 0.5 * lcs
            
            #If threshold is greater than 40, add current behaviour to the archive
            if(threshold > 40):
                #print(threshold)
                newItem = ArchiveItem(nm, self.weights, lcs, fitness)
                self.archive.append(newItem)

        #Diversity metrics
        d = [self.time_spent_small, self.time_spent_medium, self.time_spend_large]
        self.diversity.append(d)

        #Reset time spent pushing resources
        self.time_spent_small = 0
        self.time_spent_medium = 0
        self.time_spend_large = 0

        #If received archives from other robots
        if self.received_archives:
            #Flatten the list of archives
            flattenedArchives = list(chain.from_iterable(self.received_archives))
            #Get a random item from the flattened archive
            randomItem = np.random.choice(list(flattenedArchives))
            #Get the weights from the random item
            new_weights = randomItem.get_genome()
            #Mutate the weights
            new_weights = np.random.normal(new_weights, 0.1)

            #Reset metrics and clear received archive list
            self.weights = new_weights
            self.received_archives.clear()
            self.objects_deposited = 0
            self.archives_received = 0
            self.distance_travelled = 0
            self.deactivated = False
        #If haven't received arhives from other robots, deactivate robot for next generation
        else:
            self.deactivated = True