import numpy as np

#Neural network class
class NeuralNetwork:

    def __init__(self, weights, nb_inputs, nb_outputs, nb_neurons_hidden, nb_layers, bias):
        self.weights = weights
        self.nb_inputs = nb_inputs
        self.nb_outputs = nb_outputs
        self.inputs = np.zeros(nb_inputs, dtype=int)
        self.outputs = np.zeros(nb_outputs, dtype=int)
        self.nb_neurons_per_layer = []
        self.nb_neurons_per_layer.append(nb_inputs)
        for i in range(1, nb_layers-1):
            self.nb_neurons_per_layer.append(nb_neurons_hidden)
        self.nb_neurons_per_layer.append(nb_outputs)
        self.bias = bias
    
    def step(self):

        curr_layer = self.inputs
        weights_index = 0
        tmp = []

        #Loop through number of neurons per layer
        for k in range(len(self.nb_neurons_per_layer)-1):
            #Get the number of outputs for layer and initialize to 0
            nb_out = self.nb_neurons_per_layer[k+1]
            tmp = np.zeros(nb_out)

            #Loops through current layer
            for i in range(len(curr_layer)):
                #Calculate outputs
                for j in range(nb_out):
                    tmp[j] += curr_layer[i] * self.weights[weights_index]
                    weights_index += 1

            #Apply bias to outputs
            for i in range(nb_out):
                tmp[i] += self.bias

            #Apply tanh function to outputs
            for i in range(nb_out):
                tmp[i] = np.tanh(tmp[i])

            curr_layer = tmp
        
        self.outputs = tmp

    #Get the number of weights needed
    def get_nb_weights(self):
        nb_weights = 0
        for i in range(len(self.nb_neurons_per_layer)-1):
            nb_weights += self.nb_neurons_per_layer[i] * self.nb_neurons_per_layer[i+1]
        return nb_weights

    #Set the weights
    def set_weights(self, weights):
        self.weights = weights
    
    #Set the inputs
    def set_inputs(self, inputs):
        self.inputs = inputs

    #Get the outputs
    def get_outputs(self):
        return self.outputs
        