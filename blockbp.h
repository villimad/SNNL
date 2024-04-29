#ifndef BLOCKBP_H
#define BLOCKBP_H
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <stack>

enum class Operation {
    Plus,   // +
    Minus,  // -
    Mul,    // *
    Del,    // /
    VecSum, // vector sum
    Exp,    // exp(x)
    Relu,   // if (x >= 0) then x else 0
    D_Relu, // if (x >= 0) then 1 else 0
    Sigmoid,// 1 / (1 + exp(x))
    Tanh,   // ( exp(2x) - 1 ) / ( exp(2x) + 1 )
    Ln      // Natural logarifm
};


class Variable {
public:
    std::string name;
    Variable(std::string name) :
        name{name} 
    {};
};


class BlockBP
{
public:
    BlockBP(std::shared_ptr<BlockBP> block) {
        value = block->value;
        var = block->var;
        operation = block->operation;
        its_value_block = block->its_value_block;
        its_variable_block = block->its_variable_block;
        its_operation_block = block->its_operation_block;
        its_original_block = false;
        original_block = block;

        for (auto q : block->vec_blocks) {
            vec_blocks.push_back(q);
        }
    }

    BlockBP(double value)
        : value{value}
    {
        its_value_block = true;
    };

    BlockBP(std::shared_ptr<Variable> var)
        : var{var}
    {
        its_variable_block = true;
        var_list.push_back(var);
    }

    BlockBP(std::shared_ptr<BlockBP> first_block, Operation operation) // activation function
        : first_block{first_block}, operation{operation}
    {
        its_operation_block = true;
        for (auto q : first_block->var_list) {
            var_list.push_back(q);
        }
    }

    BlockBP(std::shared_ptr<BlockBP> first_block, std::shared_ptr<BlockBP> second_block, Operation operation)
        : first_block{first_block}, second_block{second_block}, operation{operation}
    {
        its_operation_block = true;
        if (first_block->var_list.size() != 0) {
            for (auto q : first_block->var_list) {
                var_list.push_back(q);
            }
        }

        if ((second_block->var_list.size() != 0)) {
            for (auto q : second_block->var_list) {
                if (std::count(var_list.begin(), var_list.end(), q) == 0) {
                    var_list.push_back(q);
                }
            }
        }
    };

    BlockBP(std::vector<std::shared_ptr<BlockBP>> vec_blocks, Operation operation)
        : operation{operation}, vec_blocks{vec_blocks}
    {
        its_operation_block = true;
        for (auto q : vec_blocks) {
            for (auto q1 : q->var_list) {
                if (std::count(var_list.begin(), var_list.end(), q1) == 0) {
                    var_list.push_back(q1);
                }
            }
        }
    }

    std::shared_ptr<BlockBP> first_block, second_block;
    double value = 0;
    Operation operation = Operation::Plus;
    std::shared_ptr<Variable> var;
    std::vector<std::shared_ptr<BlockBP>> vec_blocks {};

    bool its_variable_block = false;
    bool its_value_block = false;
    bool its_operation_block = false;
    std::vector<std::shared_ptr<Variable>> var_list {};

    bool its_original_block = true;
    std::shared_ptr<BlockBP> original_block;

    bool its_binary_operation() {
        if (its_operation_block) {
            if ((operation == Operation::Plus) ||
                (operation == Operation::Minus) ||
                (operation == Operation::Del) ||
                (operation == Operation::Mul)
                )
            {
                return true;
            }
            return false;
        } 
    }
};


class CopyBlockBP
{
public:

    std::shared_ptr<BlockBP> res_block;

    CopyBlockBP(std::shared_ptr<BlockBP> found_block) {
        res_block = std::make_shared<BlockBP>(found_block);
        run_found_block(found_block, res_block);
    }

    void run_found_block(std::shared_ptr<BlockBP> block, std::shared_ptr<BlockBP> copy_block) {
        copy_block->value = block->value;
        copy_block->var = block->var;
        copy_block->operation = block->operation;
        copy_block->its_value_block = block->its_value_block;
        copy_block->its_variable_block = block->its_variable_block;
        copy_block->its_operation_block = block->its_operation_block;

        if (block->its_operation_block) {
            if (block->operation == Operation::VecSum) {
                for (unsigned int q = 0; q < block->vec_blocks.size(); q++) {
                    std::shared_ptr<BlockBP> new_copy_block = std::make_shared<BlockBP>(block->vec_blocks.at(q));
                    copy_block->vec_blocks.at(q) = new_copy_block;
                    run_found_block(block->vec_blocks.at(q), new_copy_block);
                }
            } else {
                if (block->operation == Operation::Plus ||
                        block->operation == Operation::Mul ||
                        block->operation == Operation::Del ||
                        block->operation == Operation::Minus)
                {
                std::shared_ptr<BlockBP> first_copy_block = std::make_shared<BlockBP>(block->first_block);
                copy_block->first_block = first_copy_block;
                run_found_block(block->first_block, first_copy_block);
                std::shared_ptr<BlockBP> second_copy_block = std::make_shared<BlockBP>(block->second_block);
                copy_block->second_block = second_copy_block;
                run_found_block(block->second_block, second_copy_block);
                } else {
                    std::shared_ptr<BlockBP> first_copy_block = std::make_shared<BlockBP>(block->first_block);
                    copy_block->first_block = first_copy_block;
                    run_found_block(block->first_block, first_copy_block);
                }
            }
        }
    };

};

class BackpropRec
{
public:

    std::shared_ptr<BlockBP> top_block;
    std::vector<std::shared_ptr<Variable>> var_list;
    std::vector<
        std::pair<
            std::shared_ptr<Variable>, std::shared_ptr<BlockBP>
        >
    > vec_diff {};


    BackpropRec(std::shared_ptr<BlockBP> top_block, std::vector<std::shared_ptr<Variable>> var_list)
        :top_block{top_block}, var_list{var_list}
    {
        for (auto q : var_list) {
            std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>> res_pair = std::make_pair(q, diff(q, top_block));
            vec_diff.push_back(res_pair);
        }
    };

    std::shared_ptr<BlockBP> diffMul(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> first_block, std::shared_ptr<BlockBP> second_block)
    {
        std::shared_ptr<BlockBP> block1 = std::make_shared<BlockBP>(diff(var, first_block), second_block, Operation::Mul);
        std::shared_ptr<BlockBP> block2 = std::make_shared<BlockBP>(diff(var, second_block), first_block, Operation::Mul);
        std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(block1, block2, Operation::Plus);
        return res_block;
    }

    std::shared_ptr<BlockBP> diffDel(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> first_block, std::shared_ptr<BlockBP> second_block)
    {
        std::shared_ptr<BlockBP> block1 = std::make_shared<BlockBP>(diff(var, first_block), second_block, Operation::Mul);
        std::shared_ptr<BlockBP> block2 = std::make_shared<BlockBP>(diff(var, second_block), first_block, Operation::Mul);
        std::shared_ptr<BlockBP> block_up_side = std::make_shared<BlockBP>(block1, block2, Operation::Minus);
        std::shared_ptr<BlockBP> block_bottom_side = std::make_shared<BlockBP>(second_block, second_block, Operation::Mul);
        std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(block_up_side, block_bottom_side, Operation::Del);
        return res_block;

    }

    std::shared_ptr<BlockBP> diffPlus(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> first_block, std::shared_ptr<BlockBP> second_block)
    {
        std::shared_ptr<BlockBP> plus_block = std::make_shared<BlockBP>(diff(var, first_block), diff(var, second_block), Operation::Plus);
        return plus_block;
    }

    std::shared_ptr<BlockBP> diffMinus(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> first_block, std::shared_ptr<BlockBP> second_block)
    {
        std::shared_ptr<BlockBP> minus_block = std::make_shared<BlockBP>(diff(var, first_block), diff(var, second_block), Operation::Minus);
        return minus_block;
    }

    std::shared_ptr<BlockBP> diffVecSum(std::shared_ptr<Variable> var, std::vector<std::shared_ptr<BlockBP>> vec_blocks)
    {
        std::vector<std::shared_ptr<BlockBP>> vector_res {};
        for (auto q : vec_blocks) {
            vector_res.push_back(diff(var, q));
        }

        std::shared_ptr<BlockBP> vecsum_block = std::make_shared<BlockBP>(vector_res, Operation::VecSum);
        return vecsum_block;
    }

    std::shared_ptr<BlockBP> diffExp(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> block) {
        std::shared_ptr<BlockBP> diff_exp = std::make_shared<BlockBP>(block->first_block, Operation::Exp);
        std::shared_ptr<BlockBP> diff_under_exp = diff(var, block->first_block);
        std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(diff_exp, diff_under_exp, Operation::Mul);
        return res_block;
    }

    // Relu begin

    std::shared_ptr<BlockBP> diffRelu(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> block) {
        // Operation Diff relu will write in Solver class
        std::shared_ptr<BlockBP> diff_relu = std::make_shared<BlockBP>(block, Operation::D_Relu);
        std::shared_ptr<BlockBP> diff_under_relu = diff(var, block);
        std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(diff_relu, diff_under_relu, Operation::Mul);
        return res_block;
    }

    std::shared_ptr<BlockBP> diffD_Relu(std::shared_ptr<Variable>, std::shared_ptr<BlockBP>) {
        double value = 0;
        std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(value);
        return res_block;
    }

    // Relu end

    std::shared_ptr<BlockBP> diffLn(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> block) {
        std::shared_ptr<BlockBP> mul1 =  diff(var, block);
        std::shared_ptr<BlockBP> val = std::make_shared<BlockBP>(1);
        std::shared_ptr<BlockBP> mul2 = std::make_shared<BlockBP>(val, block, Operation::Del);
        std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(mul1, mul2, Operation::Mul);
        return res_block;
    }

    std::shared_ptr<BlockBP> diffSigmoid(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> block) {
        double minus_one = -1;
        std::shared_ptr<BlockBP> minus_one_block = std::make_shared<BlockBP>(minus_one);
        std::shared_ptr<BlockBP> under_block = std::make_shared<BlockBP>(minus_one_block, block, Operation::Mul);
        std::shared_ptr<BlockBP> exp_block = std::make_shared<BlockBP>(under_block, Operation::Exp);
        std::shared_ptr<BlockBP> inside_block = std::make_shared<BlockBP>(std::make_shared<BlockBP>(1), exp_block, Operation::Plus);
        std::shared_ptr<BlockBP> bottom_side_block = std::make_shared<BlockBP>(inside_block, inside_block, Operation::Mul);
        std::shared_ptr<BlockBP> diffSigmoid = std::make_shared<BlockBP>(exp_block, bottom_side_block, Operation::Del);
        std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(diff(var, block), diffSigmoid, Operation::Mul);
        return res_block;
    }

    std::shared_ptr<BlockBP> diff_var_block(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> block) {
        if (!block->its_variable_block) {
            throw std::exception();
        }

        if (block->var == var) {
            return std::make_shared<BlockBP>(1); // может не работать
        }

        if (block->var != var) {
            return std::make_shared<BlockBP>(0);
        }

        throw std::exception();
    }

    std::shared_ptr<BlockBP> diff_value_block(std::shared_ptr<BlockBP> block) {
        if (!block->its_value_block) {
            throw std::exception();
        }
        return std::make_shared<BlockBP>(0);
    }

    std::shared_ptr<BlockBP> diff(std::shared_ptr<Variable> var, std::shared_ptr<BlockBP> c_block) {

        CopyBlockBP copy_block = CopyBlockBP(c_block);

        std::shared_ptr<BlockBP> block = copy_block.res_block;

        if (block->its_operation_block) {
            if (block->operation == Operation::Mul) {
                return diffMul(var, block->first_block, block->second_block);
            }

            if (block->operation == Operation::Plus) {
                return diffPlus(var, block->first_block, block->second_block);
            }

            if (block->operation == Operation::Del) {
                return diffDel(var, block->first_block, block->second_block);
            }

            if (block->operation == Operation::Minus) {
                return diffMinus(var, block->first_block, block->second_block);
            }

            if (block->operation == Operation::VecSum) {
                return diffVecSum(var, block->vec_blocks);
            }

            if (block->operation == Operation::Exp) {
                return diffExp(var, block->first_block);
            }

            if (block->operation == Operation::Relu) {
                return diffRelu(var, block->first_block);
            }

            if (block->operation == Operation::D_Relu) {
                return diffD_Relu(var, block->first_block);
            }

            if (block->operation == Operation::Sigmoid) {
                return diffSigmoid(var, block->first_block);
            }

            if (block->operation == Operation::Ln) {
                return diffLn(var, block->first_block);
            }
        }

        if (block->its_variable_block) {
            return diff_var_block(var, block);
        }

        if (block->its_value_block) {
            return diff_value_block(block);
        }

        throw std::exception();
    }
};


class PrinterBlockBP
{
public:

    std::string result {};

    PrinterBlockBP(std::shared_ptr<BlockBP> block) {
        result = print_block(block);
    }

    std::string print_block(std::shared_ptr<BlockBP> block) {

        if (block->its_value_block) {
            return std::to_string(block->value);
        }

        if (block->its_variable_block) {
            return block->var->name;
        }


        if (block->its_operation_block) {
            if (block->operation == Operation::Plus) {
                return "( " + print_block(block->first_block) + " + " + print_block(block->second_block) + " )";
            }
            if (block->operation == Operation::Minus) {
                return "( " + print_block(block->first_block) + " - " + print_block(block->second_block) + " )";
            }
            if (block->operation == Operation::Mul) {
                return "( " + print_block(block->first_block) + " * " + print_block(block->second_block) + " )";
            }
            if (block->operation == Operation::Del) {
                return "( " + print_block(block->first_block) + " / " + print_block(block->second_block) + " )";
            }
            if (block->operation == Operation::VecSum) {
                std::string res = "( ";
                for (unsigned int q = 0; q < block->vec_blocks.size(); q++) {
                    if (q != 0) {
                        res += " + ";
                    }

                    res += print_block(block->vec_blocks.at(q));
                }
                res += " )";
                return res;
            }

            if (block->operation == Operation::Relu) {
                return "Relu( " + print_block(block->first_block) + " )";
            }

            if (block->operation == Operation::D_Relu) {
                return "D_Relu( " + print_block(block->first_block) + " )";
            }
        }

        throw std::exception();
    }
};

class CashSolve {
public:
    std::vector<std::pair<std::shared_ptr<BlockBP>, std::shared_ptr<BlockBP>>> cash;

    CashSolve() {
        cash = {};
    };

    CashSolve(std::vector<std::pair<std::shared_ptr<BlockBP>, std::shared_ptr<BlockBP>>> cash)
        : cash{cash}
    {}
};

class SolveBlockBP
{
public:
    std::shared_ptr<BlockBP> top_block;
    std::vector<std::pair<std::shared_ptr<Variable>, double>> variable_value {};
    std::vector<std::shared_ptr<BlockBP>> solved_blocks {};
    std::vector<std::pair<std::shared_ptr<BlockBP>, std::shared_ptr<BlockBP>>> cash {}; // pair<original block , result value block>
    
    CashSolve& cash_in;

    SolveBlockBP(std::shared_ptr<BlockBP> top_block, std::vector<std::pair<std::shared_ptr<Variable>, double>> variable_value, CashSolve& cash_in)
        : top_block{top_block}, variable_value{variable_value}, cash_in{cash_in}
    {
        cash = cash_in.cash;
        solve_start2();
    }

    void delete_from_vector(std::shared_ptr<BlockBP> elem) {
        solved_blocks.erase(std::find(solved_blocks.begin(), solved_blocks.end(), elem));
    }

    void update_cash(std::shared_ptr<BlockBP> block) {
        std::shared_ptr<BlockBP> original_block;
        std::shared_ptr<BlockBP> value_block; // ??

        if (!block->its_original_block) {
            original_block = block->original_block;
        } else {
            original_block = block;
        }

        for (auto cash_pair : cash) {
            if (cash_pair.first == original_block) {
                return;
            }
        }

        auto cash_pair = std::make_pair(original_block, block);
        cash.push_back(cash_pair);
    }

    void change_cash() {
        CashSolve new_cash = CashSolve{ cash };
        CashSolve& new_cash_pointer = new_cash;
        cash_in = new_cash_pointer;
    }

    void make_value_block(std::shared_ptr<BlockBP> block, double value) {
        block->value = value;
        block->its_value_block = true;
        block->its_variable_block = false;
        block->its_operation_block = false;
        block->var_list = {};

        // cash
        update_cash(block);
    }

    void solve_start2() {
        solve2(top_block);
        change_cash();
    }

    void binaryOperation(std::shared_ptr<BlockBP> block) {
        if (std::count(solved_blocks.begin(), solved_blocks.end(), block->first_block) == 0) {
            solve2(block->first_block);
            delete_from_vector(block);
            solve2(block);
            return;
        }

        if (std::count(solved_blocks.begin(), solved_blocks.end(), block->second_block) == 0) {
            solve2(block->second_block);
            delete_from_vector(block);
            solve2(block);
            return;
        }
    }

    void onseOperation(std::shared_ptr<BlockBP> block) {
        if (std::count(solved_blocks.begin(), solved_blocks.end(), block->first_block) == 0) {
            solve2(block->first_block);
            delete_from_vector(block);
            solve2(block);
            return;
        }
    }

    void solve2(std::shared_ptr<BlockBP> block) {

        if (std::count(solved_blocks.begin(), solved_blocks.end(), block) == 0) {
            solved_blocks.push_back(block);
        } else {
            return;
        }

        for (auto cash_pair : cash) {
            if (((cash_pair.first == block->original_block) && (!block->its_original_block)) || ((cash_pair.first == block) && (block->its_original_block)))
            {
                make_value_block(block, cash_pair.second->value);
                break;
            }
        }

        if (block->its_value_block) {
            return;
        }

        if (block->its_variable_block) {
            for (auto q : variable_value) {
                if (q.first == block->var) {
                    make_value_block(block, q.second);
                }
            }
            return;
        }

        if (block->its_operation_block) {
            if (block->operation == Operation::Mul) {
                if (block->first_block->its_value_block && block->second_block->its_value_block) {
                    double new_value = block->first_block->value * block->second_block->value;
                    make_value_block(block, new_value);
                    return;
                }

                if (    (block->first_block->its_value_block && block->first_block->value == 0) ||
                        (block->second_block->its_value_block && block->second_block->value == 0)) {
                    double new_value = 0;
                    make_value_block(block, new_value);
                    return;
                }

                binaryOperation(block);
                return;
            }

            if (block->operation == Operation::Plus) {
                if (block->first_block->its_value_block && block->second_block->its_value_block) {
                    double new_value = block->first_block->value + block->second_block->value;
                    make_value_block(block, new_value);
                    return;
                }

                // make zero moving

                binaryOperation(block);
                return;
            }

            if (block->operation == Operation::Minus) {
                if (block->first_block->its_value_block && block->second_block->its_value_block) {
                    double new_value = block->first_block->value - block->second_block->value;
                    make_value_block(block, new_value);
                    return;
                }

                // make zero moving

                binaryOperation(block);
                return;
            }

            if (block->operation == Operation::Del) {
                if (block->first_block->its_value_block && block->second_block->its_value_block) {
                    double new_value = block->first_block->value / block->second_block->value;
                    make_value_block(block, new_value);
                    return;
                }

                // make zero moving

                binaryOperation(block);
                return;
            }

            if (block->operation == Operation::VecSum) {
                for (auto block_in_vec : block->vec_blocks) {
                    solve2(block_in_vec);
                }

                double summary = 0;

                for (auto block_in_vec : block->vec_blocks) {
                    if (block_in_vec->its_value_block) {
                        summary += block_in_vec->value;
                    } else {
                        return;
                    }
                }
                make_value_block(block, summary);
                return;
            }

            if (block->operation == Operation::Relu) {
                if (block->first_block->its_value_block) {
                    double value = 0;
                    if (block->first_block->value >= 0) {
                        value = block->first_block->value;
                    }
                    make_value_block(block, value);
                    return;
                }
                onseOperation(block);
                return;
            }

            if (block->operation == Operation::D_Relu) {
                if (block->first_block->its_value_block) {
                    double value = 0;
                    if (block->first_block->value >= 0) {
                        value = 1;
                    }
                    make_value_block(block, value);
                    return;
                }
                onseOperation(block);
                return;
            }

            if (block->operation == Operation::Sigmoid) {
                if (block->first_block->its_value_block) {

                    double value = 1 / (1 + exp(-block->first_block->value));
                    make_value_block(block, value);
                    return;
                }
                onseOperation(block);
                return;
            }

            if (block->operation == Operation::Exp) {
                if (block->first_block->its_value_block) {
                    double value = exp(block->first_block->value);
                    make_value_block(block, value);
                    return;
                }
                onseOperation(block);
                return;
            }

            if (block->operation == Operation::Ln) {
                if (block->first_block->its_value_block) {
                    double value = log(block->first_block->value);
                    make_value_block(block, value);
                    return;
                }
                onseOperation(block);
                return;
            }

            throw std::exception();
        }
    }
};



#endif // BLOCKBP_H
