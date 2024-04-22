#ifndef PERSEPTRON_H
#define PERSEPTRON_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cmath>
#include "blockbp.h"
#include "newralnet.h"

template<typename TypeName>
TypeName randomNumber(TypeName n_min, TypeName n_max, unsigned int generator) {
    // Get once random <TypeName> number
    std::uniform_real_distribution<TypeName> dist{n_min, n_max};
    std::default_random_engine engn{generator};
    TypeName k = dist(engn);
    return k;
};

template<typename TypeName>
std::vector<TypeName> createWeight(int count, TypeName n_min, TypeName n_max) {
    std::vector<TypeName> weight = std::vector<TypeName>{};
    for (int q = 1; q < count + 2; q++) {
        weight.push_back( randomNumber(n_min, n_max, q));
    }
    return weight;
};

class Perseptron
{
public:

    int count_input, count_output;
    std::vector<std::pair<int, int>> between_layers;
    int count_between_neurons;


    std::vector<Neuron> output_layer {};
    std::vector<Neuron> between_layer {};

    std::shared_ptr<Layer> input_l;
    std::shared_ptr<Layer> output_l;
    std::shared_ptr<Layer> between_l;

    double edu_coef;

    std::vector< std::shared_ptr<BlockBP>> output_blocks;
    std::vector<std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>>> width_backprop_blocks;

    std::vector<std::shared_ptr<BlockBP>> exit_test_results_blocks {};
    std::vector<std::shared_ptr<Variable>> exit_variables {};
    std::vector<std::shared_ptr<BlockBP>> lose_func_vec {};

    std::vector<std::shared_ptr<Variable>> input_variables {};
    std::vector<std::pair<std::shared_ptr<Variable>, double>> weight_value;
    std::vector<std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>>> backprop_block {};

    std::shared_ptr<BlockBP> lose_func;
    CashSolve& cash = CashSolve{};


    Perseptron(int count_input, int count_output, int count_between_neurons, double edu_coef)
        :count_input{count_input}, count_output{count_output}, count_between_neurons{count_between_neurons}, edu_coef{edu_coef}
    {
        create_input_layer();
        create_output_layer();
        create_between_layer();
        create_neuralnet();
    }

    void create_input_layer() {
        std::vector<Neuron> input_layer {};
        for (int q = 0; q < count_input; q++) {
            input_layer.push_back(Neuron());
        }
        input_l = std::make_shared<Layer>(input_layer, true);
    }

    void create_output_layer() {
        for (int q = 0; q < count_output; q++) {
            output_layer.push_back(Neuron(TypeActivationFunc::Relu));
        }
        output_l = std::make_shared<Layer>(output_layer, false, true);
    }

    void create_between_layer() {
        for (int q = 0; q < count_between_neurons; q++) {
            between_layer.push_back(Neuron(TypeActivationFunc::Relu));
        }
        between_l = std::make_shared<Layer>(between_layer);
    }

    std::shared_ptr<BlockBP> make_lose_func() {
        std::vector<std::shared_ptr<BlockBP>> sum_blocks {};
        for (auto block : output_blocks) {
            std::shared_ptr<Variable> exit_test_result_var = std::make_shared<Variable>("exit");
            exit_variables.push_back(exit_test_result_var);
            std::shared_ptr<BlockBP> exit_test_result = std::make_shared<BlockBP>(exit_test_result_var);
            // exit_test_results_blocks.push_back(exit_test_result);
            std::shared_ptr<BlockBP> lose_func_onse = std::make_shared<BlockBP>(block, exit_test_result, Operation::Minus);
            std::shared_ptr<BlockBP> lose_func_square = std::make_shared<BlockBP>(lose_func_onse, lose_func_onse, Operation::Mul);
            std::shared_ptr<BlockBP> value_block = std::make_shared<BlockBP>(2);
            std::shared_ptr<BlockBP> lose_func_for_output = std::make_shared<BlockBP>(lose_func_square, value_block, Operation::Del);
            sum_blocks.push_back(lose_func_for_output);
        }

        std::shared_ptr<BlockBP> lose_func = std::make_shared<BlockBP>(sum_blocks, Operation::VecSum);

        return lose_func;
    }

    void create_neuralnet() {
        std::vector<Chain> chains_vec {};
        Chain chain1 = Chain(input_l, between_l);
        Chain chain2 = Chain(between_l, output_l);
        chains_vec.push_back(chain1);
        chains_vec.push_back(chain2);
        NeuralNet nn = NeuralNet(chains_vec);
        Translator trans = Translator(nn);

        for (auto output : trans.output_blocks) {
            output_blocks.push_back(output);
        }

        lose_func = make_lose_func();
        BackpropRec backprop = BackpropRec(lose_func, trans.weight_variables);

        for (auto pair_bp : backprop.vec_diff) {
            backprop_block.push_back(pair_bp);
        }

        weight_value = create_weight_value(trans.weight_variables);
        input_variables = trans.input_variables;
    }

    void education_step(std::vector<std::pair<std::shared_ptr<Variable>, double>> input_value, std::vector<std::pair<std::shared_ptr<Variable>, double>> test_value) {
        std::vector<std::pair<std::shared_ptr<Variable>, double>> variable_vec {};

     
        for (auto block : weight_value) {
            variable_vec.push_back(block);
        }

        for (auto block : input_value) {
            variable_vec.push_back(block);
        }

        std::vector<std::shared_ptr<BlockBP>> solved_blocks {};

        cash = CashSolve{}; // cash

        for (auto block : output_blocks) {
            CopyBlockBP copy_block = CopyBlockBP(block);

            SolveBlockBP solved_block = SolveBlockBP(copy_block.res_block, variable_vec, cash);
            solved_blocks.push_back(solved_block.top_block);
        }

        for (auto block : solved_blocks) {
            if (!block->its_value_block) {
                std::cout << "education_step() error" << std::endl;
                throw std::exception();
            }

            std::cout << "OUT : " + std::to_string(block->value) << std::endl;
        }

        for (auto block : test_value) {
            variable_vec.push_back(block);
        }

        // solve backprop

        for (auto &weight_variable_and_value : weight_value) {
            std::shared_ptr<BlockBP> backprop = find_block_for_variable(backprop_block, weight_variable_and_value.first);

            CopyBlockBP copy_backprop = CopyBlockBP(backprop);
            SolveBlockBP solved = SolveBlockBP(copy_backprop.res_block, variable_vec, cash);

            if (!solved.top_block->its_value_block) {
                std::cout << "Solved block not value" << std::endl;
                throw std::exception();
            }

//            PrinterBlockBP printer = PrinterBlockBP(solved.top_block);

//            std::cout << printer.result << std::endl;

            double new_value = weight_variable_and_value.second - edu_coef * solved.top_block->value; // maybe -

            std::cout << "Weight correct value : " << edu_coef * solved.top_block->value <<  std::endl;

            weight_variable_and_value.second = new_value;
        }

        CopyBlockBP copy_lose = CopyBlockBP(lose_func);

        SolveBlockBP solve_lose = SolveBlockBP(copy_lose.res_block, variable_vec, cash);

        std::cout << "Lose func = " << solve_lose.top_block->value << std::endl;

        std::cout << "Edu step Complited!" << std::endl;
    }

    std::shared_ptr<BlockBP> find_block_for_variable(std::vector<std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>>> vec, std::shared_ptr<Variable> variable) {
        for (auto element : vec) {
            if (element.first == variable) {
                return element.second;
            }
        }

        std::cout << "Not find block for" + variable->name << std::endl;
        throw std::exception();
    }

    std::vector<std::pair<std::shared_ptr<Variable>, double>> create_weight_value(std::vector<std::shared_ptr<Variable>> weight) {
        std::vector<std::pair<std::shared_ptr<Variable>, double>> weight_value {};
        std::vector<double> weight_numerical_value = createWeight<double>(weight.size(), 0.01, 2);

        for (unsigned int q = 0; q < weight.size(); q++) {
            weight_value.push_back(std::make_pair(weight[q], weight_numerical_value[q]));
        }
        return weight_value;
    }
};

#endif // PERSEPTRON_H
