#pragma once
#include "blockbp.h"
#include <map>

struct DiffTreeElement {
	unsigned int parent_number;
	bool diff_completed;
	std::vector<unsigned int> child_numbers;
	std::shared_ptr<BlockBP> block;

	DiffTreeElement()
	{
	}
};

struct CashTreeElement {
	unsigned int parent_number;
	std::shared_ptr<BlockBP> block;
	bool its_top_block = false;
	CashTreeElement(std::shared_ptr<BlockBP> block, unsigned int parent_number, bool its_top_block = false)
		:block{ block }
		, parent_number{ parent_number }
		, its_top_block{its_top_block}
	{
		
	}
	

};

struct TreeElement
{
	unsigned int parent_number;
	std::vector<unsigned int> child_numbers{};
	std::shared_ptr<BlockBP> block;
	bool its_top_block = false;
	TreeElement(CashTreeElement cash_element) 
	{
		parent_number = cash_element.parent_number;
		block = cash_element.block;
		its_top_block = cash_element.its_top_block;
	}

	TreeElement(std::shared_ptr<TreeElement> tree_element, unsigned int child_number) {
		parent_number = tree_element->parent_number;
		child_numbers = tree_element->child_numbers;
		child_numbers.push_back(child_number);
		block = tree_element->block;
		its_top_block = tree_element->its_top_block;
	}
}; 


class BackpropForward {
public:
	std::shared_ptr<BlockBP> top_block;
	std::vector<std::shared_ptr<TreeElement>> block_tree;

	BackpropForward(std::shared_ptr<BlockBP> top_block) 
		:top_block{top_block}
	{
		
	}

	void main() {
		create_block_tree();
	}
	
	void create_block_tree() {
		/*
			block -> CashTreeElement(with parents number) -> TreeElement(without child numbers) -> TreeElement(with child numbers)
		*/

	
		std::vector<CashTreeElement> cash_block_tree{};

		auto cash_block = CashTreeElement{top_block, 0, true};

		cash_block_tree.push_back(cash_block);

		unsigned int parent_number = 0;
		unsigned int child_number = 0;
		
		while (!cash_block_tree.empty()) {
			cash_block = *cash_block_tree.begin();
			cash_block_tree.erase(cash_block_tree.begin());
			std::shared_ptr<TreeElement> tree_element = std::make_shared<TreeElement>(cash_block);
			block_tree.push_back(tree_element);
			child_number = block_tree.size() - 1;

			if (!tree_element->its_top_block) {
				std::shared_ptr<TreeElement> rewrite_el = *(block_tree.begin() + tree_element->parent_number);
				std::shared_ptr<TreeElement> new_tree_el = std::make_shared<TreeElement>(rewrite_el, child_number);
				block_tree.insert(block_tree.begin() + tree_element->parent_number, new_tree_el);
			}

			parent_number = block_tree.size() - 1;

			if (tree_element->block->its_operation_block) {
				if (tree_element->block->its_binary_operation())
				{
					CashTreeElement cash1 = CashTreeElement{ tree_element->block->first_block, parent_number };
					CashTreeElement cash2 = CashTreeElement{ tree_element->block->second_block, parent_number };
					
					cash_block_tree.push_back(cash1);
					cash_block_tree.push_back(cash2);
					continue;
				}
				if ((!tree_element->block->its_binary_operation())
					&& (tree_element->block->operation != Operation::VecSum)
					) {
					CashTreeElement cash1 = CashTreeElement{ tree_element->block->first_block, parent_number };
					
					cash_block_tree.push_back(cash1);
					continue;
				}

				if (tree_element->block->operation == Operation::VecSum) {
					for (auto block_in_vec : tree_element->block->vec_blocks) {
						CashTreeElement cash_vec_block = CashTreeElement{ block_in_vec, parent_number };
						cash_block_tree.push_back(cash_vec_block);
					}
				}
			}

		}

	}

};