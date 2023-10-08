#include <iostream>
#include <string>
#include <typeinfo>
#include <vector>
#include <algorithm>
#include "neuron.hpp"

Neuron::Neuron(Sigmoid a_func) {
    create_text_graph_node("Sigmoid");
}

Neuron::Neuron(Relu a_func) {
    create_text_graph_node("Relu");
}

void Neuron::create_text_graph_node(std::string name_activation) {
    graph_node = "<Neuron " + name_activation + ">";
}