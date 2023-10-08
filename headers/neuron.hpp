#pragma once
#include <string>

class Sigmoid {};
class Relu{};

class Neuron
{
    public:
    Neuron(Sigmoid a_func);
    Neuron(Relu a_func);

    std::string graph_node;

    private:
    void create_text_graph_node(std::string name_activation);
};

