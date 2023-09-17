#include <iostream>
#include "neuron.hpp"
#include "sublayer.hpp"
#include "mathfunc.hpp" 

int main(void) {
    long double k = MathFunc::randomNumber<long double>(0, 1);
    std::cout << k << std::endl; 
    return 1;
};