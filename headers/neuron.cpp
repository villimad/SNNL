#include <iostream>
#include "neuron.hpp"

Neuron::Neuron(Sigmoid a_func) {
   std::cout << "It's Sigmoid" << std::endl;
};

Neuron::Neuron(Relu a_func) {
   std::cout << "It's Relu lu lu lu" << std::endl;
};