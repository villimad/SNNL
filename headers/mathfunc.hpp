#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <ctime>
#include <cmath>

namespace MathFunc {
    /*
    Collection of specific mathematical functions
    */
    template<typename TypeName>
    TypeName randomNumber(TypeName n_min, TypeName n_max, int generator) {
        // Get once random <TypeName> number
        std::uniform_real_distribution<TypeName> dist{n_min, n_max};
        std::default_random_engine engn{generator};
        TypeName k = dist(engn);
        return k; 
    };

    template<typename TypeName>
    std::vector<TypeName>* createWeight(int count, TypeName n_min, TypeName n_max) {
        std::vector<TypeName>* weight;
        for (int q = 1; q < count + 2; q++) {
            weight->push_back( randomNumber(n_min, n_max, q) * pow(-1, q) );
        }
        return weight;
    };
}