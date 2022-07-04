from pyroborobo import Controller, Pyroborobo
from archive_item import ArchiveItem
from itertools import chain
from math import atan2, degrees
from heuristics.pickup_heuristic import PickupHeuristic
import numpy as np

class NSLCController(Controller):

    def __init__(self, wm):
        super().__init__(wm)
        self.weights = None
        self.archive = list()
        self.received_archives = list()
        self.rob = Pyroborobo.get()
        self.next_gen_every_it = 400
        self.deactivated = False
        self.next_gen_in_it = self.next_gen_every_it
        self.objects_deposited = 0
        self.archives_received = 0
        self.heuristic = None
        self.wait_it = 0

    def reset(self):
        if self.weights is None:
            self.weights = np.random.uniform(-1, 1, (self.nb_inputs(), 2))
            archive_item = ArchiveItem(np.array([0, 0]), self.weights.copy(), 0, 0)
            self.archive.append(archive_item)

    def step(self):
        self.next_gen_in_it -= 1
        if self.next_gen_in_it < 0 or self.deactivated:
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
            objects_a = i[2]

            min_index = np.argmin(objects_d)
            if self.heuristic is None:
                if objects_d[min_index] < 0.5:
                    obj = self.get_object_instance_at(min_index)
                    obj_position = obj.position
                    rob_position = self.absolute_position

                    if not obj.is_bound and not obj.placed and self.wait_it == 0:
                        obj.is_bound = True
                        pickupHeuristic = PickupHeuristic(self, obj)
                        self.heuristic = pickupHeuristic

                    else:
                        trans_speed, rot_speed = inputs @ self.weights
                        self.set_translation(np.clip(trans_speed, -1, 1))
                        self.set_rotation(np.clip(rot_speed, -1, 1))
                else:
                    trans_speed, rot_speed = inputs @ self.weights
                    self.set_translation(np.clip(trans_speed, -1, 1))
                    self.set_rotation(np.clip(rot_speed, -1, 1))
                
                if self.wait_it > 0:
                    self.wait_it -= 1
            else:
                self.heuristic.step()
            # Share archive
            self.share_archive()

    def nb_inputs(self):
        return (1  # bias
                + self.nb_sensors * 3  # cam inputs
                + 2 #landmark
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
        sensors = self.get_all_sensor_angles()
        is_robots = self.get_all_robot_ids() != -1
        is_walls = self.get_all_walls()
        is_objects = self.get_all_objects() != -1

        robots_dist = np.where(is_robots, dists, 1)
        walls_dist = np.where(is_walls, dists, 1)
        objects_dist = np.where(is_objects, dists, 1)
        objects_angle = np.where(is_objects, sensors, -1)

        landmark_dist = self.get_closest_landmark_dist()
        landmark_orient = self.get_closest_landmark_orientation()

        inputs = np.concatenate([[1], robots_dist, walls_dist, objects_dist, [landmark_dist, landmark_orient]])
        assert(len(inputs) == self.nb_inputs())
        return inputs, objects_dist, objects_angle

    def new_generation(self):
        bc = np.array([self.objects_deposited, self.archives_received])
        fitness = self.objects_deposited
        archiveLen = len(self.archive)

        k = min(50, archiveLen)
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
            if(p > 80):
                newItem = ArchiveItem(bc, self.weights, lcs, fitness)
                self.archive.append(newItem)

        if self.received_archives:
            flattenedArchives = list(chain.from_iterable(self.received_archives))
            randomItem = np.random.choice(list(flattenedArchives))
            new_weights = randomItem.get_genome()

            # mutation
            new_weights = np.random.normal(new_weights, 0.1)

            self.weights = new_weights
            self.received_archives.clear()
            self.objects_deposited = 0
            self.archives_received = 0
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
        print(f"{self.id}  Objects deposited: {self.objects_deposited}")

    def vec_between(start_v, end_v, check_v):
        se_v = np.cross(start_v, end_v)

        if(se_v >= 0):
            return np.cross(start_v, check_v) >= 0 and np.cross(check_v, end_v) >= 0
        else:
            return not(np.cross(start_v, check_v) >= 0 and np.cross(check_v, end_v) >= 0)

    def moveTo(self, cur_pos, pos):
        angle = atan2(cur_pos[1] - pos[1], cur_pos[0] - pos[0])
        angle = degrees(angle)

        self.set_absolute_orientation(angle - 180)
        self.set_translation(1)
