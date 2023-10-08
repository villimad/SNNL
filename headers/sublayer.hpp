#pragma once
#include <vector>
#include "neuron.hpp"

class Chain {
public:
    explicit Chain(
            unsigned long int number_connect_sub = 0,
            bool inputChain = false,
            bool outputChain = false
            )
        : number_connect_sub{number_connect_sub}, inputChain{inputChain}, outputChain{outputChain}
    {
        if (inputChain && outputChain)
            throw std::exception();
    };
    unsigned long int number_connect_sub;
    bool inputChain;  // this sublayer - input ?
    bool outputChain; // this sublayer - output ?
};

class SubLayer {
    /*
    The SubLayers unites neurons into Layers
    */
    public:
    std::string graph_node;
    Chain connection;
    SubLayer(unsigned int global_number,Neuron neurons, unsigned int neuronsCount, Chain connection);
    private:
    unsigned int global_number;
    Neuron neurons;
    unsigned int neuronsCount;
    void textNode();
};

