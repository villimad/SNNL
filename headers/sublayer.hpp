#pragma once
#include <vector>
#include "neuron.hpp"

struct Chain;

class SubLayer {
    /*
    The SubLayers unites neurons into Layers
    */
    public:
    SubLayer(
        std::vector<Neuron> neurons,
        std::vector<unsigned long int> connect_sl, 
        unsigned long int global_number, 
        std::vector<Chain> connections
        );

    private:
    unsigned long int global_number;
    std::vector<Neuron> neurons; 
    std::vector<unsigned long int> connect_sl;
    std::vector<Chain> connections;
    ~SubLayer();
};

struct Chain {
    unsigned long int number_connect_sub;
    std::vector<double> weight;
};