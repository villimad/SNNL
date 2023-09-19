#include <iostream>
#include "neuron.hpp"
#include "sublayer.hpp"
#include "mathfunc.hpp" 

int main(void) {
    std::vector<double> *k = MathFunc::createWeight<double>(728, 0, 1);
    
    std::cout << k->at(1) << std::endl;

    return 1;
};