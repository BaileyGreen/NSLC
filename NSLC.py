from pyroborobo import Pyroborobo, Controller, MovableObject
from custom.objects import BallObject
from custom.controllers import NSLCController

def main():
    print(dir(Pyroborobo))
    rob = Pyroborobo.create("config/nslc.properties",
                            controller_class=NSLCController,
                            object_class_dict={'_default': BallObject})
    rob.start()
    rob.update(100000)
    rob.close()


if __name__ == "__main__":
    main()
