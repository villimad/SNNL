#include "sublayer.hpp"
#include "neuron.hpp"
#include <vector>

SubLayer::SubLayer(
    std::vector<Neuron> neurons, 
    std::vector<unsigned long int> connect_sl,
    unsigned long int global_number,
    std::vector<Chain> connections
    )
    :neurons{neurons}, 
    connect_sl{connect_sl},
    global_number{global_number},
    connections{connections}
{};
