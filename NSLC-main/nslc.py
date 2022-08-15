from pyroborobo import Pyroborobo, Controller, MovableObject
from custom.objects import EasyObject
from custom.controllers import NSLCController
from custom.world_observers import NSLCWorldObserver
from statistics import mean

class NSLC():
    def __init__(self):
        self.rob = Pyroborobo.create("config/nslc.properties",
                            controller_class=NSLCController,
                            world_observer_class=NSLCWorldObserver,
                            object_class_dict={'_default': EasyObject})
    def run(self):
        self.rob.start()
        self.rob.update(10000)

        combined_archive = list()
        world_obs = self.rob.world_observer

        for controller in self.rob.controllers:
            for item in controller.archive:
                if item not in combined_archive:
                    combined_archive.append(item)
        
        self.rob.close()
        return [max(item.fitness for item in combined_archive), round(mean(item.fitness for item in combined_archive), 3), len(combined_archive), world_obs.objects_placed, world_obs.end_it]

