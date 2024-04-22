#ifndef PERCEPTRON2_H
#define PERCEPTRON2_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cmath>
#include "blockbp.h"
#include "newralnet.h"
#include "perseptron.h"
#include "BlockBPDiff.h"

class Perceptron2
{
public:
    int input_size, output_size, between_size, between_layers_count;
    double edu_coef;

    std::shared_ptr<Layer> input_l;
    std::shared_ptr<Layer> output_l;


    std::vector<std::shared_ptr<Layer>> between_layers {};
    std::vector< std::shared_ptr<BlockBP>> output_blocks {};

    std::shared_ptr<BlockBP> lose_func;

    std::vector<std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>>> width_backprop_blocks;

    std::vector<std::shared_ptr<BlockBP>> exit_test_results_blocks {};
    std::vector<std::shared_ptr<Variable>> exit_variables {};
    std::vector<std::shared_ptr<BlockBP>> lose_func_vec {};

    std::vector<std::shared_ptr<Variable>> input_variables {};
    std::vector<std::pair<std::shared_ptr<Variable>, double>> weight_value;
    std::vector<std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>>> backprop_block {};

    CashSolve& cash = CashSolve{};


    Perceptron2(int input_size, int output_size, int between_size, int between_layers_count, double edu_coef)
        :input_size{input_size}, output_size{output_size}, between_size{between_size}, between_layers_count{between_layers_count}, edu_coef{edu_coef}
    {
        create_input_layer();
        create_output_layer();
        create_between_layers();
        create_neuralnet();
    }

    void create_input_layer() {
        std::vector<Neuron> input_layer {};
        for (int q = 0; q < input_size; q++) {
            input_layer.push_back(Neuron());
        }
        input_l = std::make_shared<Layer>(input_layer, true);
    }

    void create_output_layer() {
        std::vector<Neuron> output_layer {};
        for (int q = 0; q < output_size; q++) {
            output_layer.push_back(Neuron(TypeActivationFunc::Relu));
        }
        output_l = std::make_shared<Layer>(output_layer, false, true);
    }


    void create_between_layers() {
        //std::vector<std::shared_ptr<Layer>> between_layers {};

        for (int l_number = 0; l_number < between_layers_count; l_number++) {
            std::vector<Neuron> between_layer {};
            for (int q = 0; q < between_size; q++) {
                between_layer.push_back(Neuron(TypeActivationFunc::Relu));
            }
            between_layers.push_back(std::make_shared<Layer>(between_layer));
        }
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

            std::shared_ptr<BlockBP> lose_func_for_output = std::make_shared<BlockBP>(lose_func_square,  value_block, Operation::Del);
//            std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(lose_func_for_output, Operation::Ln);
            sum_blocks.push_back(lose_func_for_output);
        }

        std::shared_ptr<BlockBP> lose_func = std::make_shared<BlockBP>(sum_blocks, Operation::VecSum);
        return lose_func;
    }

    void create_neuralnet() {

        std::vector<Chain> chains_vec {};
        Chain chain1 = Chain(input_l, between_layers[0]);
        chains_vec.push_back(chain1);
        for (int q = 1; q < between_layers_count; q++) {
            Chain chain_bl = Chain(between_layers[q-1], between_layers[q]);
            chains_vec.push_back(chain_bl);
        }
        chains_vec.push_back(Chain(between_layers[between_layers_count - 1], output_l));
        NeuralNet nn = NeuralNet(chains_vec);
        Translator trans = Translator(nn);
        for (auto output : trans.output_blocks) {
            output_blocks.push_back(output);
        }
        lose_func = make_lose_func();
        BackpropForward backprop = BackpropForward(lose_func, trans.weight_variables);
        backprop.main(); // deleted if Rec

        std::cout << "Test complited" << std::endl;

        for (auto pair_bp : backprop.vec_diff) {
            backprop_block.push_back(pair_bp);
        }

        weight_value = create_weight_value(trans.weight_variables);
        input_variables = trans.input_variables;

    }

    void education_step(std::vector<std::pair<std::shared_ptr<Variable>, double>> input_value, std::vector<std::pair<std::shared_ptr<Variable>, double>> test_value) {
        std::vector<std::pair<std::shared_ptr<Variable>, double>> variable_vec {};

        cash = CashSolve{};

        for (auto block : weight_value) {
            variable_vec.push_back(block);
        }

        for (auto block : input_value) {
            variable_vec.push_back(block);
        }

        std::vector<std::shared_ptr<BlockBP>> solved_blocks {};

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

            double new_value = weight_variable_and_value.second - edu_coef * solved.top_block->value ; // maybe -
            if (edu_coef * solved.top_block->value == 0) {
                //
            };
            std::cout << "Weight value : " <<  weight_variable_and_value.second <<  std::endl;
            std::cout << "Weight correct value : " << edu_coef * solved.top_block->value <<  std::endl;

            weight_variable_and_value.second = new_value;
        }

        CopyBlockBP copy_lose2 = CopyBlockBP(lose_func);

        SolveBlockBP solve_lose2 = SolveBlockBP(copy_lose2.res_block, variable_vec, cash);

        std::cout << "Lose func2 = " << solve_lose2.top_block->value << std::endl;

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
        std::vector<double> weight_numerical_value = createWeight<double>(weight.size(), 1, 5);

        for (unsigned int q = 0; q < weight.size(); q++) {
            weight_value.push_back(std::make_pair(weight[q], weight_numerical_value[q]));
        }
        return weight_value;
    }
};

class Perceptron2Rec
{
public:
    int input_size, output_size, between_size, between_layers_count;
    double edu_coef;

    std::shared_ptr<Layer> input_l;
    std::shared_ptr<Layer> output_l;


    std::vector<std::shared_ptr<Layer>> between_layers{};
    std::vector< std::shared_ptr<BlockBP>> output_blocks{};

    std::shared_ptr<BlockBP> lose_func;

    std::vector<std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>>> width_backprop_blocks;

    std::vector<std::shared_ptr<BlockBP>> exit_test_results_blocks{};
    std::vector<std::shared_ptr<Variable>> exit_variables{};
    std::vector<std::shared_ptr<BlockBP>> lose_func_vec{};

    std::vector<std::shared_ptr<Variable>> input_variables{};
    std::vector<std::pair<std::shared_ptr<Variable>, double>> weight_value;
    std::vector<std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>>> backprop_block{};

    CashSolve& cash = CashSolve{};


    Perceptron2Rec(int input_size, int output_size, int between_size, int between_layers_count, double edu_coef)
        :input_size{ input_size }, output_size{ output_size }, between_size{ between_size }, between_layers_count{ between_layers_count }, edu_coef{ edu_coef }
    {
        create_input_layer();
        create_output_layer();
        create_between_layers();
        create_neuralnet();
    }

    void create_input_layer() {
        std::vector<Neuron> input_layer{};
        for (int q = 0; q < input_size; q++) {
            input_layer.push_back(Neuron());
        }
        input_l = std::make_shared<Layer>(input_layer, true);
    }

    void create_output_layer() {
        std::vector<Neuron> output_layer{};
        for (int q = 0; q < output_size; q++) {
            output_layer.push_back(Neuron(TypeActivationFunc::Relu));
        }
        output_l = std::make_shared<Layer>(output_layer, false, true);
    }


    void create_between_layers() {
        //std::vector<std::shared_ptr<Layer>> between_layers {};

        for (int l_number = 0; l_number < between_layers_count; l_number++) {
            std::vector<Neuron> between_layer{};
            for (int q = 0; q < between_size; q++) {
                between_layer.push_back(Neuron(TypeActivationFunc::Relu));
            }
            between_layers.push_back(std::make_shared<Layer>(between_layer));
        }
    }

    std::shared_ptr<BlockBP> make_lose_func() {
        std::vector<std::shared_ptr<BlockBP>> sum_blocks{};
        for (auto block : output_blocks) {
            std::shared_ptr<Variable> exit_test_result_var = std::make_shared<Variable>("exit");
            exit_variables.push_back(exit_test_result_var);
            std::shared_ptr<BlockBP> exit_test_result = std::make_shared<BlockBP>(exit_test_result_var);
            // exit_test_results_blocks.push_back(exit_test_result);
            std::shared_ptr<BlockBP> lose_func_onse = std::make_shared<BlockBP>(block, exit_test_result, Operation::Minus);
            std::shared_ptr<BlockBP> lose_func_square = std::make_shared<BlockBP>(lose_func_onse, lose_func_onse, Operation::Mul);
            std::shared_ptr<BlockBP> value_block = std::make_shared<BlockBP>(2);

            std::shared_ptr<BlockBP> lose_func_for_output = std::make_shared<BlockBP>(lose_func_square, value_block, Operation::Del);
            //            std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(lose_func_for_output, Operation::Ln);
            sum_blocks.push_back(lose_func_for_output);
        }

        std::shared_ptr<BlockBP> lose_func = std::make_shared<BlockBP>(sum_blocks, Operation::VecSum);
        return lose_func;
    }

    void create_neuralnet() {

        std::vector<Chain> chains_vec{};
        Chain chain1 = Chain(input_l, between_layers[0]);
        chains_vec.push_back(chain1);
        for (int q = 1; q < between_layers_count; q++) {
            Chain chain_bl = Chain(between_layers[q - 1], between_layers[q]);
            chains_vec.push_back(chain_bl);
        }
        chains_vec.push_back(Chain(between_layers[between_layers_count - 1], output_l));
        NeuralNet nn = NeuralNet(chains_vec);
        Translator trans = Translator(nn);
        for (auto output : trans.output_blocks) {
            output_blocks.push_back(output);
        }
        lose_func = make_lose_func();
        BackpropRec backprop = BackpropRec(lose_func, trans.weight_variables);

        std::cout << "BP complited" << std::endl;

        for (auto pair_bp : backprop.vec_diff) {
            backprop_block.push_back(pair_bp);
        }

        weight_value = create_weight_value(trans.weight_variables);
        input_variables = trans.input_variables;

    }

    void education_step(std::vector<std::pair<std::shared_ptr<Variable>, double>> input_value, std::vector<std::pair<std::shared_ptr<Variable>, double>> test_value) {
        std::vector<std::pair<std::shared_ptr<Variable>, double>> variable_vec{};

        cash = CashSolve{};

        for (auto block : weight_value) {
            variable_vec.push_back(block);
        }

        for (auto block : input_value) {
            variable_vec.push_back(block);
        }

        std::vector<std::shared_ptr<BlockBP>> solved_blocks{};

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


        for (auto& weight_variable_and_value : weight_value) {

            std::shared_ptr<BlockBP> backprop = find_block_for_variable(backprop_block, weight_variable_and_value.first);

            CopyBlockBP copy_backprop = CopyBlockBP(backprop);
            SolveBlockBP solved = SolveBlockBP(copy_backprop.res_block, variable_vec, cash);

            if (!solved.top_block->its_value_block) {
                std::cout << "Solved block not value" << std::endl;
                throw std::exception();
            }

            double new_value = weight_variable_and_value.second - edu_coef * solved.top_block->value; // maybe -
            if (edu_coef * solved.top_block->value == 0) {
                //
            };
            std::cout << "Weight value : " << weight_variable_and_value.second << std::endl;
            std::cout << "Weight correct value : " << edu_coef * solved.top_block->value << std::endl;

            weight_variable_and_value.second = new_value;
        }

        CopyBlockBP copy_lose2 = CopyBlockBP(lose_func);

        SolveBlockBP solve_lose2 = SolveBlockBP(copy_lose2.res_block, variable_vec, cash);

        std::cout << "Lose func2 = " << solve_lose2.top_block->value << std::endl;

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
        std::vector<std::pair<std::shared_ptr<Variable>, double>> weight_value{};
        std::vector<double> weight_numerical_value = createWeight<double>(weight.size(), 1, 5);

        for (unsigned int q = 0; q < weight.size(); q++) {
            weight_value.push_back(std::make_pair(weight[q], weight_numerical_value[q]));
        }
        return weight_value;
    }
};
#endif // PERCEPTRON2_H





















