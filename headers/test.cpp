#include <cassert>
#include <iostream>
#include <vector>
#include "network.hpp"
#include "neuron.hpp"
#include "layer.hpp"
#include "sublayer.hpp"
#include "test.h"

void Test::run() {
    neuronTextNodeTest();
    sublayerTextNodeTest();
    layerTextNodeTest();
};

void Test::neuronTextNodeTest() {
    try {
        Neuron neuron{ Relu{} };
        if (neuron.graph_node == "<Neuron Relu>") {
            std::cout << "Test neuron.hpp neuron.cpp (text node): Complited!" << std::endl;
        } else {
            std::cout << "Message: ";
            std::cout << neuron.graph_node << std::endl;
            throw std::exception();
        }
    } catch (...) {
        std::cout << "Test neuron.hpp neuron.cpp (text node): Failed!" << std::endl;
    }
};

void Test::sublayerTextNodeTest() {
    try {
        Chain chain;
        chain.inputChain = true;
        SubLayer sublayer {0, Neuron{ Relu{} }, 70, chain};
        if (sublayer.graph_node == "<Sublayer INPUT GN=0 Neuron: {<Neuron Relu>|count=70}>" ) {
            std::cout << "Test sublayer.hpp sublayer.cpp (text node): Complited!" << std::endl;
        } else {
            std::cout << "Message: ";
            std::cout << sublayer.graph_node << std::endl;
            throw std::exception();
        }
    } catch (...) {
        std::cout << "Test sublayer.hpp sublayer.cpp (text node): Failed!" << std::endl;
    }
}

void Test::layerTextNodeTest() {
    try {
        Chain chain;
        chain.inputChain = true;
        SubLayer sublayer {0, Neuron{ Relu{} }, 70, chain};
        Layer layer = Layer{std::vector<SubLayer> {sublayer}, 0};
        if (layer.graph_node == "<Layer INPUT GN=0 Sublayer: {<Sublayer INPUT GN=0 Neuron: {<Neuron Relu>|count=70}>}>") {
            std::cout << "Test layer.hpp layer.cpp (text node): Complited!" << std::endl ;
        } else {
            std::cout << "Message: ";
            std::cout << layer.graph_node << std::endl;
            throw std::exception();
        }
    } catch (...) {
        std::cout << "Test layer.hpp layer.cpp (text node): Failed!" << std::endl;
    }
}

void Test::neuralTextNodeTest() {

//    try {
//        Neuron neuron = Neuron{ Sigmoid{} };
//        Chain chain;
//        chain.inputChain = true;
//        SubLayer sublayer = SubLayer{0, neuron, 90, chain};
//        std::vector<SubLayer> vecSubLayer;
//        Layer layer1 = Layer{vecSubLayer};
//        Network network = Network{Layer};
//        if (network.graf == "InputLayer <globalnumber = 0> {Sub: <SubLayer <globalnumber = 0> 90 Sigmoid Neurons >}") {

//        } else {
//            throw std::exception();
//        }


//    } catch (...) {
//        std::cout << "Test neuralgraf: Failed!" << std::endl;
//    }

    std::cout << "Test neuralgraf: Complited!" << std::endl;
};
