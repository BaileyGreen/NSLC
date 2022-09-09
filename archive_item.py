import numpy as np

#Arhive item class
class ArchiveItem():

    def __init__(self, novelty_metric, genome, lcs, fitness):
        self.novelty_metric = novelty_metric
        self.genome = genome
        self.local_competition_score = lcs
        self.fitness = fitness

    #Get the genome (weights)
    def get_genome(self):
        return self.genome
    
    #Get the novelty metric
    def get_novelty_metric(self):
        return self.novelty_metric

    #Get the local competition score
    def get_local_competition_score(self):
        return self.local_competition_score

    #Get the fitness
    def get_fitness(self):
        return self.fitness

    #Set the novelty metric
    def set_novelty_metric(self, nm):
        self.novelty_metric = nm

    #Check if two arhive items are equal
    def __eq__(self, other):
        return np.array_equal(self.novelty_metric, other.novelty_metric) and np.array_equal(self.genome, other.genome) and self.local_competition_score == other.local_competition_score and self.fitness == other.fitness
