#include "../include/bf/bf.hpp"

#define ARITHMETIC_CASE(instruction_type)			\
	case (instruction_type):						\
	if (write_itr->type == (instruction_type)) {	\
		write_itr->data += read_itr->data;			\
	}												\
	else{											\
		++write_itr;								\
		write_itr->type = (instruction_type);		\
		write_itr->data = read_itr->data;			\
	}												\
	break;

#define PASSTHROUGH_CASE(instruction_type)		\
	case (instruction_type):					\
		++write_itr;							\
		write_itr->type = (instruction_type);	\
	break;

namespace bf::opt {
	std::vector<Instruction> optimize_instructions(const std::vector<Instruction>& instructions) {
		std::vector<Instruction> optimized_instructions(instructions.size());

		auto read_itr = instructions.begin();
		auto write_itr =  optimized_instructions.begin();

		while (read_itr != instructions.end()) {
			switch (read_itr->type) {
				ARITHMETIC_CASE(InstructionType::AddPtr);
				ARITHMETIC_CASE(InstructionType::AddVal);

				PASSTHROUGH_CASE(InstructionType::Print);
				PASSTHROUGH_CASE(InstructionType::Read);
				PASSTHROUGH_CASE(InstructionType::EndLoop);

			case InstructionType::BeginLoop:
				// The BeginLoop instruction presents many of the biggest opportunities for optimization

				// This version just optimizes the common case of [>] and [<]
				++read_itr;
				if (read_itr->type == InstructionType::AddPtr) {
					++read_itr;
					if (read_itr->type == InstructionType::EndLoop) {
						++write_itr;
						write_itr->type = InstructionType::MovePointerToZeroValue;
						write_itr->data = (read_itr - 1)->data;
					}
					else {
						--read_itr;
						--read_itr;

						++write_itr;
						write_itr->type = InstructionType::BeginLoop;
					}
				}
				else {
					--read_itr;

					++write_itr;
					write_itr->type = InstructionType::BeginLoop;
				}
				break;

			default:;
			}

			++read_itr;
		}

		optimized_instructions.erase(++write_itr);

		return optimized_instructions;
	}
}
