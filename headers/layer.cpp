#include <iostream>
#include "layer.hpp"

Layer::Layer(std::vector<SubLayer> vecSubLayer, int global_number)
    :vecSubLayer{vecSubLayer}, global_number{global_number}
{
    textNode();
}

void Layer::textNode() {
    graph_node += "<Layer";
    if (detectInputChain())
        graph_node += " INPUT";
    if (detectOutputChain())
        graph_node += " OUTPUT";
    graph_node += " GN=" + std::to_string(global_number);
    graph_node += " Sublayer:";
    graph_node += " {";
    for (unsigned int q=0; q < vecSubLayer.size(); q++) {
        graph_node += vecSubLayer[q].graph_node;
        if ((vecSubLayer.size() > 1) && ( q != vecSubLayer.size() - 1)) {
            graph_node += ", ";
        }
    }
    graph_node += "}>";
}

bool Layer::detectInputChain() {
    for (const auto &q : vecSubLayer) {
        if (q.connection.inputChain) {
            return true;
        }
    }
    return false;
}

bool Layer::detectOutputChain() {
    for (const auto &q : vecSubLayer) {
        if (q.connection.outputChain) {
            return true;
        }
    }
    return false;
}
