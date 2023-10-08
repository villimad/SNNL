#include "sublayer.hpp"
#include <string>

SubLayer::SubLayer(
        unsigned int global_number,
        Neuron neurons,
        unsigned int neuronsCount,
        Chain connection
        )
    :global_number{global_number}, neurons{neurons}, neuronsCount{neuronsCount}, connection{connection}
{
    textNode();
};

void SubLayer::textNode() {
    std::string neuronTextNode = neurons.graph_node;
    graph_node += "<Sublayer";
    if (connection.inputChain) {
        graph_node += " INPUT";
    }
    if (connection.outputChain) {
        graph_node += " OUTPUT";
    }
    graph_node += " GN=" + std::to_string(global_number);
    graph_node += " Neuron: {" + neurons.graph_node + "|" + "count=" +std::to_string(neuronsCount) + "}>";
}
