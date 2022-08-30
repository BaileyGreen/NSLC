from pyroborobo import Pyroborobo, Controller, MovableObject
from custom.objects import EasyObject
from custom.controllers import mEDEAController
from custom.world_observers import NSLCWorldObserver
from statistics import mean

class MEDEA():
    def __init__(self):
        self.rob = Pyroborobo.create("config/nslc.properties",
                            controller_class=mEDEAController,
                            world_observer_class=NSLCWorldObserver,
                            object_class_dict={'_default': EasyObject})
    def run(self):
        self.rob.start()
        self.rob.update(10100)

        combined_archive = list()
        world_obs = self.rob.world_observer

        for controller in self.rob.controllers:
            for item in controller.archive:
                if item not in combined_archive:
                    combined_archive.append(item)
        
        times = list()
        for controller in self.rob.controllers:
            times.append(controller.diversity)
      
        nb_rob = len(times)
        nb_gen = len(times[0])

        diversity = list()

        for i in range(nb_gen):
            avg_a = 0
            avg_b = 0
            avg_c = 0
            for j in range(nb_rob):
                avg_a += times[j][i][0]
                avg_b += times[j][i][1]
                avg_c += times[j][i][2]
            avg_a = round(avg_a/nb_rob,2)
            avg_b = round(avg_b/nb_rob,2)
            avg_c = round(avg_c/nb_rob,2)

            d = [i+1, avg_a, avg_b, avg_c, world_obs.distances_to_gather[i]]
            diversity.append(d)
        
        performance = [max(item.fitness for item in combined_archive), round(mean(item.fitness for item in combined_archive), 3), len(combined_archive), world_obs.objects_placed, world_obs.end_it]
        self.rob.close()
        return [performance, diversity]

