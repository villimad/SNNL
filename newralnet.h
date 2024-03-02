#ifndef NEWRALNET_H
#define NEWRALNET_H

#include <memory>
#include <vector>
#include <algorithm>
#include "blockbp.h"

enum class TypeActivationFunc {
    Sigmoid, Relu
};

class Neuron
{
public:
    Operation op_func;
    Neuron(TypeActivationFunc activation_func)
    {
        if (activation_func == TypeActivationFunc::Sigmoid) {
            op_func = Operation::Sigmoid;
        }
        if (activation_func == TypeActivationFunc::Relu) {
            op_func = Operation::Relu;
        }
    }
    Neuron() {}
};

class Layer
{
public:
    std::vector<Neuron> neurons;
    bool input = false;
    bool output = false;
    Layer(std::vector<Neuron> neurons, bool input = false, bool output = false)
        : neurons{neurons}, input{input}, output{output}
    {

    }
};

class Chain
{
public:
    std::shared_ptr<Layer> first_layer;
    std::shared_ptr<Layer> second_layer;
    Chain(std::shared_ptr<Layer> first_layer, std::shared_ptr<Layer> second_layer)
        :first_layer{first_layer}, second_layer{second_layer}
    {

    }
};

class NeuralNet
{
public:
    std::vector<Chain> chain_vector;
    NeuralNet(std::vector<Chain> chain_vector)
        :chain_vector{chain_vector}
    {

    }
};

struct LayersWeight
{
    std::shared_ptr<Layer> first_layer;
    std::shared_ptr<Layer> second_layer;


    std::vector<std::vector<std::shared_ptr<BlockBP>>> weight {}; // [number_first_layer][number_second_layer]
    std::shared_ptr<BlockBP> neuron_displace;

    LayersWeight(std::shared_ptr<Layer> first_layer, std::shared_ptr<Layer> second_layer, std::vector<std::vector<std::shared_ptr<BlockBP>>> weight, std::shared_ptr<BlockBP> neuron_displace)
        :first_layer{first_layer}, second_layer{second_layer}, weight{weight}, neuron_displace{neuron_displace}
    {

    }
};


class Translator
{
public:
    NeuralNet neuralnet;
    std::vector<LayersWeight> LayersWeightVec {};
    std::shared_ptr<Layer> input_layer;
    std::shared_ptr<Layer> output_layer;
    std::vector<std::shared_ptr<BlockBP>> input_neurons {};
    std::vector<
            std::pair<std::shared_ptr<Layer>, std::vector<std::shared_ptr<BlockBP>>>
    > complited_second_layer {};

    std::vector<
            std::pair<std::shared_ptr<Layer>, std::vector<std::shared_ptr<BlockBP>>>
    > complited_first_layer {};

    std::vector<std::shared_ptr<BlockBP>> output_blocks {};
    std::vector<std::shared_ptr<Variable>> weigth_variables {};
    std::vector<std::shared_ptr<Variable>> input_variables {};


    Translator(NeuralNet neuralnet)
        : neuralnet{neuralnet}
    {
        chains_to_weight();
        find_input_output_layers();
        start_blockbp_macker();
    }

    void chains_to_weight() {
        for (auto q : neuralnet.chain_vector) {
            chain_to_blockbp(q);
        }
    }

    // find input and output layers
    void find_input_output_layers() {
        for (auto q : LayersWeightVec) {
            if (q.first_layer->input) {
                input_layer = q.first_layer;
                break;
            } 
        }
        for (auto q : LayersWeightVec) {
            if (q.second_layer->output) {
                output_layer = q.second_layer;
                break;
            }
        }
    }

    void chain_to_blockbp(Chain chain)
    {      
        // translate chain to LayersWidth
        // added displace neuron and create BlockBP weight

        int neuron_size_first_layer = chain.first_layer->neurons.size();
        int neuron_size_second_layer = chain.second_layer->neurons.size();

        std::vector<std::shared_ptr<BlockBP>> blocks_arr_first_layer {};
        std::vector<std::shared_ptr<BlockBP>> blocks_arr_second_layer {};
        std::vector<std::vector<std::shared_ptr<BlockBP>>> weight {};

        for (int q = 0; q < neuron_size_first_layer; q++) {
            std::vector<std::shared_ptr<BlockBP>> layers_weight {};
            for (int q1 = 0; q1 < neuron_size_second_layer; q1++) {
                std::string name_weight = "[" + std::to_string(q) + "]" + "[" + std::to_string(q1) + "]";
                std::shared_ptr<Variable> ones_weight = std::make_shared<Variable>(name_weight);
                weigth_variables.push_back(ones_weight);
                std::shared_ptr<BlockBP> block_wight = std::make_shared<BlockBP>(ones_weight);
                layers_weight.push_back(block_wight);
            }
            weight.push_back(layers_weight);
        }

        std::shared_ptr<Variable> displace_var = std::make_shared<Variable>("Neuron displace");
        weigth_variables.push_back(displace_var);
        std::shared_ptr<BlockBP> neuron_displace = std::make_shared<BlockBP>(displace_var);

        LayersWeight layers_chain = LayersWeight(chain.first_layer, chain.second_layer, weight, neuron_displace);
        LayersWeightVec.push_back(layers_chain);
    }

    std::vector<LayersWeight> weight_finder(std::shared_ptr<Layer> first_layer) {
        std::vector<LayersWeight> weight_vec {};
        for (auto q : LayersWeightVec) {
            if (q.first_layer == first_layer) {
                weight_vec.push_back(q);
            }
        }
        return weight_vec;
    }

    std::vector<LayersWeight> second_neuron_weight_finder(std::shared_ptr<Layer> second_layer) {
        std::vector<LayersWeight> weight_vec {};
        for (auto q : LayersWeightVec) {
            if (q.second_layer == second_layer) {
                weight_vec.push_back(q);
            }
        }
        return weight_vec;
    }

    bool  layer_ready_for_write(std::shared_ptr<Layer> layer) {
        int weights = second_neuron_weight_finder(layer).size();
        int complited_weight = 0;
        for (auto q : complited_second_layer) {
            if (q.first == layer) {
                complited_weight++;
            }
        }
        if (complited_weight == weights) {
            return true;
        }
        return false;
    }

    void start_blockbp_macker() {
        blockbp_maker(input_layer);
    }

    void blockbp_maker(std::shared_ptr<Layer> layer) {

        std::vector<LayersWeight> layers_weight_system = weight_finder(layer);

        if (not layer_ready_for_write(layer)) return;

        // sum and activation neurons in first layer

        std::vector<std::shared_ptr<BlockBP>> first_layer_neurons {};

        if (layer == input_layer) {
            for (unsigned int input_number = 0; input_number < layer->neurons.size(); input_number++) {
                std::string name_var = "i" + std::to_string(input_number);
                std::shared_ptr<Variable> input_neuron_var = std::make_shared<Variable>(name_var);
                input_variables.push_back(input_neuron_var);
                std::shared_ptr<BlockBP> input_neuron = std::make_shared<BlockBP>(input_neuron_var);
                first_layer_neurons.push_back(input_neuron);
            }
        } else {
            for (unsigned int q = 0; q < layer->neurons.size(); q++) {
                std::vector<std::shared_ptr<BlockBP>> sum_neurons {};
                for (auto comp_layer : complited_second_layer) {
                    if (comp_layer.first == layer) {
                        sum_neurons.push_back(comp_layer.second.at(q));
                    }
                }
                std::shared_ptr<BlockBP> neuron_sum = std::make_shared<BlockBP>(sum_neurons, Operation::VecSum);
                std::shared_ptr<BlockBP> neuron_activ = std::make_shared<BlockBP>(neuron_sum, layer->neurons.at(q).op_func);
                first_layer_neurons.push_back(neuron_activ);
            }
        }

        complited_first_layer.push_back(std::make_pair(layer, first_layer_neurons));

        if (layer == output_layer) {
            for (auto output_neurons : first_layer_neurons) {
                output_blocks.push_back(output_neurons);
            }
            return;
        }

        for (unsigned int q = 0; q < layers_weight_system.size(); q++) {
            std::vector<std::shared_ptr<BlockBP>> second_layer {};
            for (unsigned int number_sec_neuron = 0; number_sec_neuron < layers_weight_system[q].second_layer->neurons.size(); number_sec_neuron++) {
                std::vector<std::shared_ptr<BlockBP>> sum_neurons {};
                for (unsigned int first_neuron_number = 0; first_neuron_number < first_layer_neurons.size(); first_neuron_number++ ) {
                    std::shared_ptr<BlockBP> weight_neuron = std::make_shared<BlockBP>(first_layer_neurons[first_neuron_number], layers_weight_system[q].weight.at(first_neuron_number)[number_sec_neuron], Operation::Mul);
                    sum_neurons.push_back(weight_neuron);
                }
                sum_neurons.push_back(layers_weight_system.at(q).neuron_displace);
                std::shared_ptr<BlockBP> second_neuron = std::make_shared<BlockBP>(sum_neurons, Operation::VecSum);
                second_layer.push_back(second_neuron);
            }
            complited_second_layer.push_back(std::make_pair(layers_weight_system[q].second_layer, second_layer));
            blockbp_maker(layers_weight_system[q].second_layer);
        }
    }
};

#endif // NEWRALNET_H
