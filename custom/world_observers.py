from pyroborobo import WorldObserver

class NSLCWorldObserver(WorldObserver):
    
    COLLECTION_START = (0, 300)
    COLLECTION_END = (100, 400)

    def __init__(self, world):
        super().__init__(world)

    def init_pre(self):
        super().init_pre()

    def init_post(self):
        super().init_post()

    def step_post(self):
        super().step_post()

    def step_pre(self):
        super().step_pre()