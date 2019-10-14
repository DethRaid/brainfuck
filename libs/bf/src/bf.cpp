#include "../include/bf/bf.hpp"

#define ARITHMETIC_CASE(instruction_type)			\
	case (instruction_type):						\
	if (write_itr->type == (instruction_type)) {	\
		write_itr->data++;							\
		++read_itr;									\
	}												\
	else{											\
		++write_itr;								\
		write_itr->type = (instruction_type);		\
		write_itr->data = 1;						\
	}												\
	break;

#define PASSTHROUGH_CASE(instruction_type)	\
	case (instruction_type):				\
		*write_itr = *read_itr;				\
		++read_itr;							\
		++write_itr;						\
	break;



namespace bf::opt {
	std::vector<Instruction> optimize_instructions(const std::vector<Instruction>& instructions) {
		std::vector<Instruction> optimized_instructions(instructions.size());

		auto read_itr = instructions.begin();
		auto write_itr = optimized_instructions.begin();

		while (read_itr != instructions.end()) {
			switch (read_itr->type) {
				ARITHMETIC_CASE(InstructionType::AddPtr);
				ARITHMETIC_CASE(InstructionType::SubPtr);
				ARITHMETIC_CASE(InstructionType::AddVal);
				ARITHMETIC_CASE(InstructionType::SubVal);

				PASSTHROUGH_CASE(InstructionType::Print);
				PASSTHROUGH_CASE(InstructionType::Read);
				PASSTHROUGH_CASE(InstructionType::BeginLoop);
				PASSTHROUGH_CASE(InstructionType::EndLoop);
				
				PASSTHROUGH_CASE(InstructionType::InstructionCount);

			default:;
			}
		}

		return optimized_instructions;
	}
}
