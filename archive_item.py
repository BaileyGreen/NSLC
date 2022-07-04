class ArchiveItem():

    def __init__(self, behaviour_char, genome, lcs, fitness):
        self.behaviour_char = behaviour_char
        self.genome = genome
        self.local_competition_score = lcs
        self.fitness = fitness

    def get_genome(self):
        return self.genome
    
    def get_behaviour_char(self):
        return self.behaviour_char

    def get_local_competition_score(self):
        return self.local_competition_score

    def get_fitness(self):
        return self.fitness

    def set_behaviour_char(self, bc):
        self.behaviour_char = bc