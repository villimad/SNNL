#include <vector>
#include <string>

#include "./sublayer.hpp"

class Layer
{

public:
    std::vector<SubLayer> vecSubLayer;
    std::string graph_node;
    int global_number;
    explicit Layer(std::vector<SubLayer> vecSubLayer, int global_number);
private:
    bool detectInputChain();
    bool detectOutputChain();
    void textNode();

};
