from pyroborobo import Pyroborobo, Controller, MovableObject
from custom.objects import SmallObject
from custom.nslc_controller import NSLCController
from custom.medea_controller import mEDEAController
from custom.world_observers import NSLCWorldObserver
from statistics import mean

#Method class
class Method():
    def __init__(self, method):
        #If NSLC Method
        if method == "0":
            self.rob = Pyroborobo.create("config/nslc.properties",
                                controller_class=NSLCController,
                                world_observer_class=NSLCWorldObserver,
                                object_class_dict={'_default': SmallObject})
        #If mEDEA Method
        else:
            self.rob = Pyroborobo.create("config/nslc.properties",
                            controller_class=mEDEAController,
                            world_observer_class=NSLCWorldObserver,
                            object_class_dict={'_default': SmallObject})
    def run(self):
        #Start simulation
        self.rob.start()
        self.rob.update(10100)
        
        #Create a combined archive of all the archives
        combined_archive = list()
        world_obs = self.rob.world_observer

        for controller in self.rob.controllers:
            for item in controller.archive:
                if item not in combined_archive:
                    combined_archive.append(item)

        #Get a list of all the times spent pushing resources
        times = list()
        for controller in self.rob.controllers:
            times.append(controller.diversity)

        #Get the number of robots and number of generations
        nb_rob = len(times)
        nb_gen = len(times[0])

        #Create a list for the diversity results
        diversity = list()

        #Get the average time spent for each generation and each resource
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

            #Populate the diversity list
            d = [i+1, avg_a, avg_b, avg_c, world_obs.distances_to_gather[i]]
            diversity.append(d)
        
        #Populate the performance list
        performance = [max(item.fitness for item in combined_archive), round(mean(item.fitness for item in combined_archive), 3), len(combined_archive), world_obs.objects_placed, world_obs.end_it]
        self.rob.close()
        #Return the diversity and performance results
        return [performance, diversity]

