#include "test.h"
#include "blockbp.h"
#include "newralnet.h"
#include "perseptron.h"
#include <iostream>

using namespace std;

void Test::run() {
    diff();
    diffVecSum();
    reluSimpleTest();
    diffReluSimpleTest();
    translatorSimpleTest();
//    solveVecSumHardTest();
    mulSimpleTest();
    mulSimpleTestZero();
    plusMulSimpleTestZero();
    plusMulSimpleTest();
    perseptronTest();
}


void Test::diff() {
    std::shared_ptr<Variable> variable1 = std::make_shared<Variable>("x");
    std::shared_ptr<Variable> variable2 = std::make_shared<Variable>("y");

    std::shared_ptr<BlockBP> var1 = std::make_shared<BlockBP>(variable1);
    std::shared_ptr<BlockBP> var2 = std::make_shared<BlockBP>(variable2);
    std::shared_ptr<BlockBP> value1 = std::make_shared<BlockBP>(7);
    std::shared_ptr<BlockBP> value2 = std::make_shared<BlockBP>(8);

    std::shared_ptr<BlockBP> block1 = std::make_shared<BlockBP>(value1, var1, Operation::Mul);
    std::shared_ptr<BlockBP> block2 = std::make_shared<BlockBP>(value2, var2, Operation::Mul);
    std::shared_ptr<BlockBP> block3 = std::make_shared<BlockBP>(block1, block2, Operation::Plus);
    std::shared_ptr<BlockBP> block4 = std::make_shared<BlockBP>(block3, var1, Operation::Mul);

    BackpropRec kek = BackpropRec{block4, block4->var_list};

    CopyBlockBP copy_block = CopyBlockBP{kek.vec_diff.at(0).second};

    std::pair<std::shared_ptr<Variable>, double> pair1 = std::make_pair(variable1, 2);
    std::pair<std::shared_ptr<Variable>, double> pair2 = std::make_pair(variable2, 3);

    vector<std::pair<std::shared_ptr<Variable>, double>> vec = {pair1, pair2};

    SolveBlockBP kek_solved = SolveBlockBP(copy_block.res_block, vec);

    PrinterBlockBP pr1 = PrinterBlockBP{kek_solved.top_block};

    if ( copy_block.res_block->value == 52) {
        cout << "+ Test diff() Complited!" << endl;
    } else {
        cout << "- Test diff() Failed!" << endl;
    }
}

void Test::diffVecSum() {
    std::shared_ptr<Variable> variable1 = std::make_shared<Variable>("x");
    std::shared_ptr<Variable> variable2 = std::make_shared<Variable>("y");

    std::shared_ptr<BlockBP> var1 = std::make_shared<BlockBP>(variable1);
    std::shared_ptr<BlockBP> var2 = std::make_shared<BlockBP>(variable2);
    std::shared_ptr<BlockBP> value1 = std::make_shared<BlockBP>(7);
    std::shared_ptr<BlockBP> value2 = std::make_shared<BlockBP>(8);

    std::shared_ptr<BlockBP> block4 = std::make_shared<BlockBP>(var1, value1, Operation::Mul);
    std::shared_ptr<BlockBP> block1 = std::make_shared<BlockBP>(var1, block4, Operation::Mul);
    std::vector<std::shared_ptr<BlockBP>> heh_vec = {block1, var2, value2};
    std::shared_ptr<BlockBP> block2 = std::make_shared<BlockBP>(heh_vec, Operation::VecSum);

    BackpropRec kek = BackpropRec{block2, block2->var_list};

    CopyBlockBP copy_block = CopyBlockBP{kek.vec_diff.at(0).second};

    std::pair<std::shared_ptr<Variable>, double> pair1 = std::make_pair(variable1, 3);
    vector<std::pair<std::shared_ptr<Variable>, double>> vec = {pair1};
    SolveBlockBP kek_solved = SolveBlockBP(copy_block.res_block, vec);

    if (kek_solved.top_block->value == 42) {
        cout << "+ Test diffVecSum() Complited!" << endl;
    } else {
        cout << "- Test diffVecSum() Failed!" << endl;
    }

}

void Test::reluSimpleTest() {
    std::shared_ptr<Variable> variable1 = std::make_shared<Variable>("x");
    std::shared_ptr<BlockBP> var1 = std::make_shared<BlockBP>(variable1);
    std::shared_ptr<BlockBP> value_block = std::make_shared<BlockBP>(5);
    std::shared_ptr<BlockBP> op_block1 = std::make_shared<BlockBP>(var1, Operation::Relu);
    std::shared_ptr<BlockBP> op_block2 = std::make_shared<BlockBP>(op_block1, value_block, Operation::Plus);

    std::pair<std::shared_ptr<Variable>, double> pair1 = std::make_pair(variable1, 2);
    vector<std::pair<std::shared_ptr<Variable>, double>> vec = {pair1};

    CopyBlockBP copy_block = CopyBlockBP(op_block2);

    SolveBlockBP solved = SolveBlockBP(copy_block.res_block, vec);

    if (solved.top_block->value == 7) {
        cout << "+ Test reluSimpleTest() Complited!" << endl;
    } else {
        cout << "- Test reluSimpleTest() Failed!" << endl;
    }
}

void Test::diffReluSimpleTest() {
    std::shared_ptr<Variable> variable1 = std::make_shared<Variable>("x");
    std::shared_ptr<BlockBP> var1 = std::make_shared<BlockBP>(variable1);
    std::shared_ptr<BlockBP> value_block = std::make_shared<BlockBP>(5);
    std::shared_ptr<BlockBP> op_block1 = std::make_shared<BlockBP>(var1, Operation::Relu);
    std::shared_ptr<BlockBP> op_block2 = std::make_shared<BlockBP>(op_block1, value_block, Operation::Plus);

    BackpropRec backprop = BackpropRec(op_block2, op_block2->var_list);

    std::pair<std::shared_ptr<Variable>, double> pair1 = std::make_pair(variable1, 2);
    vector<std::pair<std::shared_ptr<Variable>, double>> vec = {pair1};

    CopyBlockBP copy_block = CopyBlockBP(backprop.vec_diff.at(0).second);

    SolveBlockBP solved = SolveBlockBP(copy_block.res_block, vec);

    if (solved.top_block->value == 1) {
        cout << "+ Test diffReluSimpleTest() Complited!" << endl;
    } else {
        cout << "- Test diffReluSimpleTest() Failed!" << endl;
    }
}

void Test::translatorSimpleTest() {
    vector<Neuron> input_layer {};
    for (int q = 0; q < 1; q++) {
        input_layer.push_back(Neuron());
    }

    vector<Neuron> between_layer {};
    for (int q = 0; q < 1; q++) {
        between_layer.push_back(Neuron(TypeActivationFunc::Relu));
    }

    vector<Neuron> output_layer {};
    for (int q = 0; q < 1; q++) {
        output_layer.push_back(Neuron(TypeActivationFunc::Relu));
    }

    std::shared_ptr<Layer> input_l = std::make_shared<Layer>(input_layer, true);
    std::shared_ptr<Layer> between_l = std::make_shared<Layer>(between_layer);
    std::shared_ptr<Layer> output_l = std::make_shared<Layer>(output_layer, false, true);

    Chain chain1 = Chain(input_l, between_l);
    Chain chain2 = Chain(between_l, output_l);

    vector<Chain> chains = {chain1, chain2};
    NeuralNet nn = NeuralNet(chains);

    Translator trans = Translator(nn);

    BackpropRec backprop = BackpropRec(trans.output_blocks.at(0), trans.output_blocks.at(0)->var_list);

    std::pair<std::shared_ptr<Variable>, double> pair1 = std::make_pair(trans.output_blocks.at(0)->var_list.at(0), 2);
    vector<std::pair<std::shared_ptr<Variable>, double>> vec = {pair1};

    CopyBlockBP copy_block = CopyBlockBP(backprop.vec_diff.at(0).second);

//    PrinterBlockBP printer_block1 = PrinterBlockBP(copy_block.res_block);

//    cout << printer_block1.result << endl;

    SolveBlockBP solved = SolveBlockBP(backprop.vec_diff.at(0).second, vec);

//    PrinterBlockBP printer_block = PrinterBlockBP(solved.top_block);

//    cout << printer_block.result << endl;

    cout << "+ Test build translatorSimpleTest() Complited!" << endl;
}

// ( ( ( ( 1.000000 * [0][1] ) + ( 0.000000 * i0 ) ) + ( ( 0.000000 * [1][1] ) + ( 0.000000 * i1 ) ) + 0.000000 ) )  not working

void Test::solveVecSumHardTest() {
    std::shared_ptr<Variable> var1 = std::make_shared<Variable>("var1");
    std::shared_ptr<Variable> var2 = std::make_shared<Variable>("var2");
    std::shared_ptr<Variable> var3 = std::make_shared<Variable>("var3");

    std::shared_ptr<BlockBP> variable1 = std::make_shared<BlockBP>(var1);
    std::shared_ptr<BlockBP> variable2 = std::make_shared<BlockBP>(var2);
    std::shared_ptr<BlockBP> variable3 = std::make_shared<BlockBP>(var3);

    std::shared_ptr<BlockBP> value1 = std::make_shared<BlockBP>(0);
    std::shared_ptr<BlockBP> value2 = std::make_shared<BlockBP>(0);

    std::shared_ptr<BlockBP> mul1 = std::make_shared<BlockBP>(value1, variable1, Operation::Mul);
    std::shared_ptr<BlockBP> mul2 = std::make_shared<BlockBP>(variable2, value2, Operation::Mul);

    std::shared_ptr<BlockBP> relu1 = std::make_shared<BlockBP>(value1, variable1, Operation::Relu);
    std::shared_ptr<BlockBP> relu2 = std::make_shared<BlockBP>(variable2, value2, Operation::Relu);

    std::vector<std::shared_ptr<BlockBP>> sum_vec {relu1, relu2};

    std::shared_ptr<BlockBP> vector_sum0 = std::make_shared<BlockBP>(sum_vec, Operation::VecSum);

    std::vector<std::shared_ptr<BlockBP>> sum_vec2 {vector_sum0, variable3};

    std::shared_ptr<BlockBP> vector_sum = std::make_shared<BlockBP>(sum_vec2, Operation::VecSum);

    std::shared_ptr<BlockBP> relu_block = std::make_shared<BlockBP>(vector_sum, Operation::D_Relu);

    CopyBlockBP copy_block = CopyBlockBP(relu_block);

    SolveBlockBP solved = SolveBlockBP(copy_block.res_block, {});

    PrinterBlockBP printer_block = PrinterBlockBP(solved.top_block);

    cout << printer_block.result << endl;

}

void Test::mulSimpleTest() {
    std::shared_ptr<Variable> variable1 = std::make_shared<Variable>("x");
    std::shared_ptr<Variable> variable2 = std::make_shared<Variable>("y");

    std::shared_ptr<BlockBP> var1 = std::make_shared<BlockBP>(variable1);
    std::shared_ptr<BlockBP> var2 = std::make_shared<BlockBP>(variable2);
    std::shared_ptr<BlockBP> value1 = std::make_shared<BlockBP>(7);
    std::shared_ptr<BlockBP> value2 = std::make_shared<BlockBP>(8);

    std::shared_ptr<BlockBP> block1 = std::make_shared<BlockBP>(value1, var1, Operation::Mul);
    std::shared_ptr<BlockBP> block2 = std::make_shared<BlockBP>(value2, var2, Operation::Mul);
    std::shared_ptr<BlockBP> block3 = std::make_shared<BlockBP>(block1, block2, Operation::Mul);

    std::vector<std::pair<std::shared_ptr<Variable>, double>> vec_var {};

    vec_var.push_back(std::make_pair(variable1, 4));
    vec_var.push_back(std::make_pair(variable2, 5));

    SolveBlockBP solved = SolveBlockBP(block3, vec_var); // 1120

    if (solved.top_block->value == 1120) {
        cout << "+ Test mulSimpleTest() Complited!" << endl;
    } else {
        cout << "- Test mulSimpleTest() Failed!" << endl;
    }
}

void Test::mulSimpleTestZero() {
    std::shared_ptr<Variable> variable1 = std::make_shared<Variable>("x");
    std::shared_ptr<Variable> variable2 = std::make_shared<Variable>("y");

    std::shared_ptr<BlockBP> var1 = std::make_shared<BlockBP>(variable1);
    std::shared_ptr<BlockBP> var2 = std::make_shared<BlockBP>(variable2);
    std::shared_ptr<BlockBP> value1 = std::make_shared<BlockBP>(7);
    std::shared_ptr<BlockBP> value2 = std::make_shared<BlockBP>(8);

    std::shared_ptr<BlockBP> block1 = std::make_shared<BlockBP>(value1, var1, Operation::Mul);
    std::shared_ptr<BlockBP> block2 = std::make_shared<BlockBP>(value2, var2, Operation::Mul);
    std::shared_ptr<BlockBP> block3 = std::make_shared<BlockBP>(block1, block2, Operation::Mul);

    std::vector<std::pair<std::shared_ptr<Variable>, double>> vec_var {};

    vec_var.push_back(std::make_pair(variable1, 4));
    vec_var.push_back(std::make_pair(variable2, 0));

    SolveBlockBP solved = SolveBlockBP(block3, vec_var); // 0

    if (solved.top_block->value == 0) {
        cout << "+ Test mulSimpleTest2() Complited!" << endl;
    } else {
        cout << "- Test mulSimpleTest2() Failed!" << endl;
    }
}

void Test::plusMulSimpleTestZero() {
    std::shared_ptr<Variable> variable1 = std::make_shared<Variable>("x");
    std::shared_ptr<Variable> variable2 = std::make_shared<Variable>("y");

    std::shared_ptr<BlockBP> var1 = std::make_shared<BlockBP>(variable1);
    std::shared_ptr<BlockBP> var2 = std::make_shared<BlockBP>(variable2);
    std::shared_ptr<BlockBP> value1 = std::make_shared<BlockBP>(7);
    std::shared_ptr<BlockBP> value2 = std::make_shared<BlockBP>(8);

    std::shared_ptr<BlockBP> block1 = std::make_shared<BlockBP>(value1, var1, Operation::Mul);
    std::shared_ptr<BlockBP> block2 = std::make_shared<BlockBP>(value2, var2, Operation::Mul);
    std::shared_ptr<BlockBP> block3 = std::make_shared<BlockBP>(block1, block2, Operation::Mul);

    std::shared_ptr<BlockBP> value3 = std::make_shared<BlockBP>(0);
    std::shared_ptr<BlockBP> block4 = std::make_shared<BlockBP>(block3, value3, Operation::Plus);

    std::vector<std::pair<std::shared_ptr<Variable>, double>> vec_var {};

    vec_var.push_back(std::make_pair(variable1, 4));
    vec_var.push_back(std::make_pair(variable2, 5));

    SolveBlockBP solved = SolveBlockBP(block4, vec_var); // 1120

    if (solved.top_block->value == 1120) {
        cout << "+ Test plusMulSimpleTestZero() Complited!" << endl;
    } else {
        cout << "- Test plusMulSimpleTestZero() Failed!" << endl;
    }
}

void Test::plusMulSimpleTest() {
    std::shared_ptr<Variable> variable1 = std::make_shared<Variable>("x");
    std::shared_ptr<Variable> variable2 = std::make_shared<Variable>("y");

    std::shared_ptr<BlockBP> var1 = std::make_shared<BlockBP>(variable1);
    std::shared_ptr<BlockBP> var2 = std::make_shared<BlockBP>(variable2);
    std::shared_ptr<BlockBP> value1 = std::make_shared<BlockBP>(7);
    std::shared_ptr<BlockBP> value2 = std::make_shared<BlockBP>(8);

    std::shared_ptr<BlockBP> block1 = std::make_shared<BlockBP>(value1, var1, Operation::Mul);
    std::shared_ptr<BlockBP> block2 = std::make_shared<BlockBP>(value2, var2, Operation::Mul);
    std::shared_ptr<BlockBP> block3 = std::make_shared<BlockBP>(block1, block2, Operation::Mul);

    std::shared_ptr<BlockBP> value3 = std::make_shared<BlockBP>(80);
    std::shared_ptr<BlockBP> block4 = std::make_shared<BlockBP>(block3, value3, Operation::Plus);

    std::vector<std::pair<std::shared_ptr<Variable>, double>> vec_var {};

    vec_var.push_back(std::make_pair(variable1, 4));
    vec_var.push_back(std::make_pair(variable2, 5));

    CopyBlockBP copy_block = CopyBlockBP(block4);

    SolveBlockBP solved = SolveBlockBP(copy_block.res_block, vec_var); // 1120


    if (solved.top_block->value == 1200) {
        cout << "+ Test plusMulSimpleTest() Complited!" << endl;
    } else {
        cout << "- Test plusMulSimpleTest() Failed!" << endl;
    }
}

double parabola(double z) {
    return 0.2 * z;
}

void Test::perseptronTest() {
    Perseptron perseptron = Perseptron(1, 1, 20, 0.001);
    auto exit_variables = perseptron.exit_variables;
    auto input_variables = perseptron.input_variables;

    for (int q1 = 0; q1 < 10; q1++) {
        for (int q = 0; q < 10; q++) {
            std::cout << "IN : " << q << std::endl;
            std::vector<std::pair<std::shared_ptr<Variable>, double>> test_var_value = {};
            test_var_value.push_back(std::make_pair(exit_variables[0], parabola(q)));
            std::vector<std::pair<std::shared_ptr<Variable>, double>> input_var_value = {};
            input_var_value.push_back(std::make_pair(input_variables[0], q));

            perseptron.education_step(input_var_value, test_var_value);
        }
    }

}








