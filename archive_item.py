class ArchiveItem():

    def __init__(self, behaviour_char, genome):
        self.behaviour_char = behaviour_char
        self.genome = genome

    def get_genome(self):
        return self.genome
    
    def get_behaviour_char(self):
        return self.behaviour_char

    def set_behaviour_char(self, bc):
        self.behaviour_char = bc