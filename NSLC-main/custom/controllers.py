from pyroborobo import Controller, Pyroborobo
from archive_item import ArchiveItem
from itertools import chain
from math import atan2, degrees, sqrt
from heuristics.pickup_heuristic import PickupHeuristic
from heuristics.collision_avoidance_heuristic import CAHeuristic
from neuralnetwork.neuralnetwork import NeuralNetwork
import numpy as np

class mEDEAController(Controller):

    def __init__(self, wm):
        super().__init__(wm)
        self.weights = None
        self.received_weights = dict()
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

    def reset(self):
        if self.weights is None:
            self.weights = np.random.uniform(-1, 1, self.nn.get_nb_weights())
            self.nn.set_weights(self.weights)
            archive_item = ArchiveItem(np.array([0, 0]), self.weights.copy(), 0, 0)
            self.archive.append(archive_item)

    def step(self):
        self.next_gen_in_it -= 1
        if self.next_gen_in_it < 0:
            self.new_generation()
            self.next_gen_in_it = self.next_gen_every_it

        if self.deactivated:
            self.set_color(0, 0, 0)
            self.set_translation(0)
            self.set_rotation(0)
        else:
            self.set_color(0, 0, 255)
            # Movement
            i = self.get_inputs()
            inputs = i[0]
            objects_d = i[1]

            min_index = np.argmin(objects_d)
            if self.heuristic is None:
                if objects_d[min_index] < 0.5:
                    obj = self.get_object_instance_at(min_index)
                    obj_position = obj.position
                    rob_position = self.absolute_position

                    if obj.nb_bound < type(obj).ROBOTS_REQ and not obj.placed and self.wait_it == 0:
                        obj.nb_bound += 1
                        pickupHeuristic = PickupHeuristic(self, obj)
                        self.heuristic = pickupHeuristic

                    else:
                        caHeuristic = CAHeuristic(self, obj)
                        self.heuristic = caHeuristic
                else:
                    self.nn.set_inputs(inputs)
                    self.nn.set_weights(self.weights)
                    self.nn.step()
                    out = self.nn.get_outputs()
                    trans_speed, rot_speed = out
                    self.set_translation(trans_speed)
                    self.set_rotation(rot_speed)
                
                if self.wait_it > 0:
                    self.wait_it -= 1
            else:
                self.heuristic.step()
            
            # Share weights
            self.share_weights()

            cur_pos = self.absolute_position
            self.distance_travelled += sqrt((cur_pos[0] - self.prev_pos[0])**2 + (cur_pos[1] - self.prev_pos[1])**2)
            self.prev_pos = cur_pos

    def nb_inputs(self):
        return (self.nb_sensors * 4 # cam inputs
                )

    def share_weights(self):
        for robot_controller in self.get_all_robot_controllers():
            if robot_controller:
                robot_controller.receive_weights(self.id, self.weights)

    def receive_weights(self, rid, weights):
        self.received_weights[rid] = (weights.copy())

    def get_inputs(self):
        dists = self.get_all_distances()
        is_robots = self.get_all_robot_ids() != -1
        is_walls = self.get_all_walls()
        is_objects = self.get_all_objects() != -1

        robots_dist = np.where(is_robots, dists, 1)
        walls_dist = np.where(is_walls, dists, 1)
        objects_dist = np.where(is_objects, dists, 1)

        #inputs = np.concatenate([robots_dist, walls_dist, objects_dist])
        inputs = np.concatenate([dists, is_robots, is_objects, is_walls])
        assert(len(inputs) == self.nb_inputs())
        return inputs, objects_dist

    def new_generation(self):
        bc = np.array([self.objects_deposited, self.distance_travelled])
        fitness = self.objects_deposited
        newItem = ArchiveItem(bc, self.weights, None, fitness)
        self.archive.append(newItem)

        d = [self.time_spent_small, self.time_spent_medium, self.time_spend_large]
        self.diversity.append(d)

        self.time_spent_small = 0
        self.time_spent_medium = 0
        self.time_spend_large = 0
        
        if self.received_weights:
            new_weights_key = np.random.choice(list(self.received_weights.keys()))
            new_weights = self.received_weights[new_weights_key]
            # mutation
            new_weights = np.random.normal(new_weights, 0.1)
            self.objects_deposited = 0
            self.distance_travelled = 0
            self.weights = new_weights
            self.received_weights.clear()
            self.deactivated = False
        else:
            self.deactivated = True

    def inspect(self, prefix=""):
        output = "inputs: \n" + str(self.get_inputs()) + "\n\n"
        output += "archive: \n"
        output += str(len(self.archive))
        return output

    def increment_objects_deposited(self):
        self.objects_deposited += 1


class NSLCController(Controller):

    def __init__(self, wm):
        super().__init__(wm)
        self.weights = None
        self.archive = list()
        self.received_archives = list()
        self.rob = Pyroborobo.get()
        self.nn = NeuralNetwork([], self.nb_inputs(), 2, 8, 2, 1)
        self.next_gen_every_it = 1000
        self.deactivated = False
        self.next_gen_in_it = self.next_gen_every_it
        self.objects_deposited = 0
        self.archives_received = 0
        self.heuristic = None
        self.wait_it = 0
        self.distance_travelled = 0
        self.time_spent_small = 0
        self.time_spent_medium = 0
        self.time_spend_large = 0
        self.diversity = []
        self.prev_pos = self.absolute_position

    def reset(self):
        if self.weights is None:
            self.weights = np.random.uniform(-1, 1, self.nn.get_nb_weights())
            self.nn.set_weights(self.weights)
            archive_item = ArchiveItem(np.array([0, 0]), self.weights.copy(), 0, 0)
            self.archive.append(archive_item)

    def step(self):
        self.next_gen_in_it -= 1
        if self.next_gen_in_it < 0:
            self.new_generation()
            self.next_gen_in_it = self.next_gen_every_it

        if self.deactivated:
            self.set_color(0, 0, 0)
            self.set_translation(0)
            self.set_rotation(0)
        else:
            self.set_color(0, 0, 255)
            # Movement
            i = self.get_inputs()
            inputs = i[0]
            objects_d = i[1]

            min_index = np.argmin(objects_d)
            if self.heuristic is None:
                if objects_d[min_index] < 0.5:
                    obj = self.get_object_instance_at(min_index)
                    obj_position = obj.position
                    rob_position = self.absolute_position

                    if obj.nb_bound < type(obj).ROBOTS_REQ and not obj.placed and self.wait_it == 0:
                        obj.nb_bound += 1
                        pickupHeuristic = PickupHeuristic(self, obj)
                        self.heuristic = pickupHeuristic

                    else:
                        caHeuristic = CAHeuristic(self, obj)
                        self.heuristic = caHeuristic
                else:
                    self.nn.set_inputs(inputs)
                    self.nn.set_weights(self.weights)
                    self.nn.step()
                    out = self.nn.get_outputs()
                    trans_speed, rot_speed = out
                    self.set_translation(trans_speed)
                    self.set_rotation(rot_speed)
                
                if self.wait_it > 0:
                    self.wait_it -= 1
            else:
                self.heuristic.step()
            
            # Share archive
            self.share_archive()

            cur_pos = self.absolute_position
            self.distance_travelled += sqrt((cur_pos[0] - self.prev_pos[0])**2 + (cur_pos[1] - self.prev_pos[1])**2)
            self.prev_pos = cur_pos

    def nb_inputs(self):
        return (self.nb_sensors * 4 # cam inputs
                )

    def share_archive(self):
        for robot_controller in self.get_all_robot_controllers():
            if robot_controller:
                robot_controller.receive_archives(self.archive)
                self.archives_received += 1

    def receive_archives(self, archive):
        self.received_archives.append(archive)

    def get_inputs(self):
        dists = self.get_all_distances()
        is_robots = self.get_all_robot_ids() != -1
        is_walls = self.get_all_walls()
        is_objects = self.get_all_objects() != -1

        robots_dist = np.where(is_robots, dists, 1)
        walls_dist = np.where(is_walls, dists, 1)
        objects_dist = np.where(is_objects, dists, 1)

        #inputs = np.concatenate([robots_dist, walls_dist, objects_dist])
        inputs = np.concatenate([dists, is_robots, is_objects, is_walls])
        assert(len(inputs) == self.nb_inputs())
        return inputs, objects_dist

    def new_generation(self):
        bc = np.array([self.objects_deposited, self.distance_travelled])
        fitness = self.objects_deposited
        archiveLen = len(self.archive)

        k = min(200, archiveLen)
        if archiveLen > 0:
            bdist = list()
            for item in self.archive:
                bdist.append([np.linalg.norm(bc - item.get_behaviour_char()), item.fitness])

            bdist.sort(key=lambda bdist:bdist[0])

            sum = 0
            lcs = 0
            for i in range (k):
                sum = sum + bdist[i][0]
                if bdist[i][1] < fitness:
                    lcs += 1
            
            p = sum/k
            threshold = 0.5 * p + 0.5 * lcs
            
            if(threshold > 40):
                #print(threshold)
                newItem = ArchiveItem(bc, self.weights, lcs, fitness)
                self.archive.append(newItem)

        d = [self.time_spent_small, self.time_spent_medium, self.time_spend_large]
        self.diversity.append(d)

        self.time_spent_small = 0
        self.time_spent_medium = 0
        self.time_spend_large = 0

        if self.received_archives:
            flattenedArchives = list(chain.from_iterable(self.received_archives))
            randomItem = np.random.choice(list(flattenedArchives))
            new_weights = randomItem.get_genome()
            
            new_weights = np.random.normal(new_weights, 0.1)

            self.weights = new_weights
            self.received_archives.clear()
            self.objects_deposited = 0
            self.archives_received = 0
            self.distance_travelled = 0
            self.deactivated = False
        else:
            self.deactivated = True

    def inspect(self, prefix=""):
        output = "inputs: \n" + str(self.get_inputs()) + "\n\n"
        output += "archive: \n"
        output += str(len(self.archive))
        return output

    def increment_objects_deposited(self):
        self.objects_deposited += 1
