#pragma once
#include "blockbp.h"


struct BlockBPDiff
{
    bool diff_complited = false;
    std::shared_ptr<BlockBP> block;
    std::shared_ptr<BlockBP> diff_block; 
    

    BlockBPDiff(std::shared_ptr<BlockBP> block)
        : block{ block }
    {};

    BlockBPDiff(std::shared_ptr<BlockBPDiff> old_blockdiff, std::shared_ptr<BlockBP> diff_block) 
        : block{ old_blockdiff->block }
        , diff_block{ diff_block }
        , diff_complited{ true }
    {};
};



class BackpropForward
{
public:
    std::shared_ptr<BlockBP> top_block;
    std::vector<std::shared_ptr<Variable>> var_list;
    std::vector<
        std::pair<
            std::shared_ptr<Variable>, std::shared_ptr<BlockBP>
        >
    > vec_diff{};

    std::vector<std::shared_ptr<BlockBPDiff>> block_tree{};
    std::vector<std::shared_ptr<BlockBPDiff>> working_block_tree{};


    BackpropForward(std::shared_ptr<BlockBP> top_block, std::vector<std::shared_ptr<Variable>> var_list)
        :top_block{ top_block }, var_list{ var_list }
    {};

    void main() {
        createBlockTree();
        for (int q = 0; q < var_list.size(); q++) {
            diff(var_list[q]);
            std::cout << "BP complited " << (q + 1) * 100 / var_list.size() << "%" << std::endl;
        }
    }

private:
    void createBlockTree() {
        block_tree.push_back(std::make_shared<BlockBPDiff>(top_block));
        std::vector<std::shared_ptr<BlockBP>> blocks_in_work{top_block};

        while (!blocks_in_work.empty()) {
            std::shared_ptr<BlockBP> block = *(blocks_in_work.begin());
            blocks_in_work.erase(blocks_in_work.begin());

            if (block->its_variable_block) {
                continue;
            }

            if (block->its_value_block) {
                continue;
            }

            if (block->its_operation_block) {
                if () {
                
                }
            }
        }


    }

    void diff(std::shared_ptr<Variable> var) {
        working_block_tree = block_tree; 

        /*
        //while (all_diff_complited == false) {
        //    all_diff_complited = true;
        //    for (int q = working_block_tree.size() - 1; q > -1; q--) {

        //        if (working_block_tree.at(q)->diff_complited) {
        //            continue;
        //        }

        //        all_diff_complited = false;
        //        
        //        // diff part
        //        if (working_block_tree.at(q)->block->its_value_block) {
        //            diffValue(var, working_block_tree.at(q));
        //            break;
        //        }
        //        if (working_block_tree.at(q)->block->its_variable_block) {
        //            diffVariable(var, working_block_tree.at(q));
        //            break;
        //        }
        //        if (working_block_tree.at(q)->block->its_operation_block) {
        //            // apruving ???
        //            if (working_block_tree.at(q)->block->its_binary_operation()) {
        //                if (!(diff_apruv(working_block_tree.at(q)->block->first_block)
        //                    && diff_apruv(working_block_tree.at(q)->block->second_block)))
        //                {
        //                    continue;
        //                }
        //            }
        //            
        //            std::vector<BlockBP> vec{}; //

        //            if ((!(working_block_tree.at(q)->block->its_binary_operation()))
        //                && (working_block_tree.at(q)->block->operation == Operation::VecSum))  
        //            {
        //                bool apruving_vec = true;
        //                for (auto elem_vec : working_block_tree.at(q)->block->vec_blocks) {
        //                    for (auto elem_tree : working_block_tree) {
        //                        if (elem_tree->block == elem_vec && !elem_tree->diff_complited) {
        //                            apruving_vec = false;
        //                            break;
        //                        }
        //                    }
        //                }
        //                if (apruving_vec == false) continue;
        //            }

        //            std::cout << q << std::endl;

        //            if (!(working_block_tree.at(q)->block->its_binary_operation())
        //                && (working_block_tree.at(q)->block->operation != Operation::VecSum))
        //            {
        //                if (!(diff_apruv(working_block_tree.at(q)->block->first_block))) continue;
        //            }

        //            if (working_block_tree.at(q)->block->operation == Operation::VecSum) {
        //                diffVecSum(var, working_block_tree.at(q));
        //                break;
        //            }

        //            if (working_block_tree.at(q)->block->operation == Operation::Mul) {
        //                diffMul(var, working_block_tree.at(q));
        //                break;
        //            }
        //            if (working_block_tree.at(q)->block->operation == Operation::Plus) {
        //                diffPlus(var, working_block_tree.at(q));
        //                break;
        //            }
        //            if (working_block_tree.at(q)->block->operation == Operation::Del) {
        //                diffDel(var, working_block_tree.at(q));
        //                break;
        //            }
        //            if (working_block_tree.at(q)->block->operation == Operation::Minus) {
        //                diffMinus(var, working_block_tree.at(q));
        //                break;
        //            }
        //            if (working_block_tree.at(q)->block->operation == Operation::Relu) {
        //                diffRelu(var, working_block_tree.at(q));
        //                break;
        //            }
        //            if (working_block_tree.at(q)->block->operation == Operation::Sigmoid) {
        //                diffSigmoid(var, working_block_tree.at(q));
        //                break;
        //            }
        //            if (working_block_tree.at(q)->block->operation == Operation::Ln) {
        //                diffLn(var, working_block_tree.at(q));
        //                break;
        //            }
        //        }
        //    }    
        //}

        */

        for (unsigned int q = working_block_tree.size() - 1; q > -1; q--) {
            std::shared_ptr<BlockBPDiff> tree_block = working_block_tree.at(q); // no diff_complited

            if (working_block_tree.at(q)->block->its_value_block) {
                diffValue(var, working_block_tree.at(q), q);
                break;
            }

            if (working_block_tree.at(q)->block->its_variable_block) {
                diffVariable(var, working_block_tree.at(q), q);
                break;
            }

            if (working_block_tree.at(q)->block->operation == Operation::VecSum) {
                diffVecSum(var, working_block_tree.at(q), q);
                break;
            }
            if (working_block_tree.at(q)->block->operation == Operation::Mul) {
                diffMul(var, working_block_tree.at(q), q);
                break;
            }
            if (working_block_tree.at(q)->block->operation == Operation::Plus) {
                diffPlus(var, working_block_tree.at(q), q);
                break;
            }
            if (working_block_tree.at(q)->block->operation == Operation::Del) {
                diffDel(var, working_block_tree.at(q), q);
                break;
            }
            if (working_block_tree.at(q)->block->operation == Operation::Minus) {
                diffMinus(var, working_block_tree.at(q), q);
                break;
            }
            if (working_block_tree.at(q)->block->operation == Operation::Relu) {
                diffRelu(var, working_block_tree.at(q), q);
                break;
            }
            if (working_block_tree.at(q)->block->operation == Operation::Sigmoid) {
                diffSigmoid(var, working_block_tree.at(q), q);
                break;
            }
            if (working_block_tree.at(q)->block->operation == Operation::Ln) {
                diffLn(var, working_block_tree.at(q), q);
                break;
            }
        }

        std::pair<std::shared_ptr<Variable>, std::shared_ptr<BlockBP>> result_pair = std::make_pair(var, working_block_tree.at(working_block_tree.size() - 1)->block);
        vec_diff.push_back(result_pair);
    }

    bool diff_apruv(std::shared_ptr<BlockBP> block) {
        return true;
    }

    void diffVariable(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        
        std::shared_ptr<BlockBP> res_block;

        if (found_block->block->var == var) {
            res_block = std::make_shared<BlockBP>(1);
        } else {
            res_block = std::make_shared<BlockBP>(0);
        }

        std::shared_ptr<BlockBPDiff> diff_block = std::make_shared<BlockBPDiff>(found_block, res_block);

        working_block_tree.erase( working_block_tree.begin() + number );
        working_block_tree.insert( working_block_tree.begin() + number, diff_block );
    }

    void diffValue(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        std::shared_ptr<BlockBP> res_block = std::make_shared<BlockBP>(0);
        std::shared_ptr<BlockBPDiff> diff_block = std::make_shared<BlockBPDiff>(found_block, res_block);
        working_block_tree.erase(working_block_tree.begin() + number);
        working_block_tree.insert(working_block_tree.begin() + number, diff_block);
    }

    void diffMul(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        
        std::shared_ptr<BlockBP> first_block = found_block->block->first_block;
        std::shared_ptr<BlockBP> second_block = found_block->block->second_block;

        std::shared_ptr<BlockBPDiff> first_tree_element = working_block_tree.at(number + 1);
        std::shared_ptr<BlockBPDiff> second_tree_element = working_block_tree.at(number + 2);
       
        std::shared_ptr<BlockBP> first_block_diff = working_block_tree.at(number + 1)->diff_block;
        std::shared_ptr<BlockBP> second_block_diff = working_block_tree.at(number + 2)->diff_block;

        std::shared_ptr<BlockBP> left_block  = std::make_shared<BlockBP>(first_block_diff, second_block, Operation::Mul);
        std::shared_ptr<BlockBP> right_block = std::make_shared<BlockBP>(second_block_diff, first_block, Operation::Mul);
        std::shared_ptr<BlockBP> end_block = std::make_shared<BlockBP>(left_block, right_block, Operation::Plus);
        
        std::shared_ptr<BlockBPDiff> result_block = std::make_shared<BlockBPDiff>(found_block, end_block);

        working_block_tree.erase(working_block_tree.begin() + number);
        working_block_tree.insert(working_block_tree.begin() + number, result_block);
    }

    void diffPlus(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        
        std::shared_ptr<BlockBP> first_block = found_block->block->first_block;
        std::shared_ptr<BlockBP> second_block = found_block->block->second_block;

        std::shared_ptr<BlockBPDiff> first_tree_element = working_block_tree.at(number + 1);
        std::shared_ptr<BlockBPDiff> second_tree_element = working_block_tree.at(number + 2);

        std::shared_ptr<BlockBP> first_block_diff = working_block_tree.at(number + 1)->diff_block;
        std::shared_ptr<BlockBP> second_block_diff = working_block_tree.at(number + 2)->diff_block;

        std::shared_ptr<BlockBP> end_block = std::make_shared<BlockBP>(first_block_diff, second_block_diff, Operation::Plus);

        std::shared_ptr<BlockBPDiff> result_block = std::make_shared<BlockBPDiff>(found_block, end_block);

        working_block_tree.erase(working_block_tree.begin() + number);
        working_block_tree.insert(working_block_tree.begin() + number, result_block);
    }

    void diffDel(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        
        std::shared_ptr<BlockBP> first_block = found_block->block->first_block;
        std::shared_ptr<BlockBP> second_block = found_block->block->second_block;

        std::shared_ptr<BlockBPDiff> first_tree_element = working_block_tree.at(number + 1);
        std::shared_ptr<BlockBPDiff> second_tree_element = working_block_tree.at(number + 2);

        std::shared_ptr<BlockBP> first_block_diff = working_block_tree.at(number + 1)->diff_block;
        std::shared_ptr<BlockBP> second_block_diff = working_block_tree.at(number + 2)->diff_block;

        std::shared_ptr<BlockBP> left_block = std::make_shared<BlockBP>(first_block_diff, second_block, Operation::Mul);
        std::shared_ptr<BlockBP> right_block = std::make_shared<BlockBP>(second_block_diff, first_block, Operation::Mul);
        std::shared_ptr<BlockBP> up_side_block = std::make_shared<BlockBP>(left_block, right_block, Operation::Minus);

        std::shared_ptr<BlockBP> bottom_side_block = std::make_shared<BlockBP>(second_block, second_block, Operation::Mul);
        std::shared_ptr<BlockBP> end_block = std::make_shared<BlockBP>(up_side_block, bottom_side_block, Operation::Del);

        std::shared_ptr<BlockBPDiff> result_block = std::make_shared<BlockBPDiff>(found_block, end_block);

        working_block_tree.erase(working_block_tree.begin() + number);
        working_block_tree.insert(working_block_tree.begin() + number, result_block);
    }

    void diffMinus(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        std::shared_ptr<BlockBP> first_block = found_block->block->first_block;
        std::shared_ptr<BlockBP> second_block = found_block->block->second_block;

        std::shared_ptr<BlockBPDiff> first_tree_element = working_block_tree.at(number + 1);
        std::shared_ptr<BlockBPDiff> second_tree_element = working_block_tree.at(number + 2);

        std::shared_ptr<BlockBP> first_block_diff = working_block_tree.at(number + 1)->diff_block;
        std::shared_ptr<BlockBP> second_block_diff = working_block_tree.at(number + 2)->diff_block;

        std::shared_ptr<BlockBP> end_block = std::make_shared<BlockBP>(first_block_diff, second_block_diff, Operation::Minus);

        std::shared_ptr<BlockBPDiff> result_block = std::make_shared<BlockBPDiff>(found_block, end_block);

        working_block_tree.erase(working_block_tree.begin() + number);
        working_block_tree.insert(working_block_tree.begin() + number, result_block);
    }
    
    void diffVecSum(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        
        std::vector<std::shared_ptr<BlockBP>> diff_vec{};

        for (unsigned int q = 0; q < found_block->block->vec_blocks.size(); q++) {
            diff_vec.push_back(working_block_tree.at(number + 1 + q)->diff_block);
        }

        std::shared_ptr<BlockBP> end_block = std::make_shared<BlockBP>(diff_vec, Operation::VecSum);

        

        std::shared_ptr<BlockBPDiff> result_block = std::make_shared<BlockBPDiff>(found_block, end_block);

        working_block_tree.erase(working_block_tree.begin() + number);
        working_block_tree.insert(working_block_tree.begin() + number, result_block);
    };

    void diffExp(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        std::shared_ptr<BlockBP> first_block = found_block->block->first_block;

        std::shared_ptr<BlockBPDiff> first_tree_element = working_block_tree.at(number + 1);

        std::shared_ptr<BlockBP> first_block_diff = working_block_tree.at(number + 1)->diff_block;


        std::shared_ptr<BlockBP> diff_exp = std::make_shared<BlockBP>(found_block->block->first_block, Operation::Exp);
        std::shared_ptr<BlockBP> end_block = std::make_shared<BlockBP>(first_block_diff, diff_exp, Operation::Mul);
        std::shared_ptr<BlockBPDiff> result_block = std::make_shared<BlockBPDiff>(found_block, end_block);

        working_block_tree.erase(working_block_tree.begin() + number);
        working_block_tree.insert(working_block_tree.begin() + number, result_block);
     };

    void diffRelu(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
        std::shared_ptr<BlockBP> first_block = found_block->block->first_block;

        std::shared_ptr<BlockBPDiff> first_tree_element = working_block_tree.at(number + 1);

        std::shared_ptr<BlockBP> first_block_diff = working_block_tree.at(number + 1)->diff_block;

        std::shared_ptr<BlockBP> diff_relu = std::make_shared<BlockBP>(first_block, Operation::D_Relu);
        std::shared_ptr<BlockBP> end_block = std::make_shared<BlockBP>(diff_relu, first_block_diff, Operation::Mul);
        std::shared_ptr<BlockBPDiff> result_block = std::make_shared<BlockBPDiff>(found_block, end_block);

        auto it_tree = find(working_block_tree.begin(), working_block_tree.end(), found_block);

        if (it_tree == working_block_tree.end()) throw std::exception("Element out off working block tree");

        working_block_tree.erase(it_tree);
        working_block_tree.push_back(result_block);
    };

    void diffLn(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {
    
    };

    void diffSigmoid(std::shared_ptr<Variable> var, std::shared_ptr<BlockBPDiff> found_block, unsigned int number) {

    };

};