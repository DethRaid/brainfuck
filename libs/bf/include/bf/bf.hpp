#pragma once

#include <cstdint>
#include <iostream>
#include <vector>

namespace bf::opt {
	enum class InstructionType : uint8_t {
		// Basic Brainfuck instructions
		AddPtr,		// data is the number to add 
		AddVal,		// data is the number to add
		Print,		// data ignored
		Read,		// data ignored
		BeginLoop,	// data ignored
		EndLoop,	// data ignored

		// Optimized instructions
		MovePointerToZeroValue,
		AddNearbyVal,
		SubNearbyVal,
		
		InstructionCount,
	};

	struct Instruction {
		int8_t data = 0;
		InstructionType type = InstructionType::InstructionCount;
	};

	static_assert(sizeof(Instruction) == 2);

	inline Instruction parse_token(const char token) {
		// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
		switch (token) {
		case '>':
			return Instruction{
				1,
				InstructionType::AddPtr
			};

		case '<':
			return Instruction{
				-1,
				InstructionType::AddPtr
			};

		case '+':
			return Instruction{
				1,
				InstructionType::AddVal
			};

		case '-':
			return Instruction{
				-1,
				InstructionType::AddVal
			};

		case '.':
			return Instruction{
				0,
				InstructionType::Print
			};

		case ',':
			return Instruction{
				0,
				InstructionType::Read
			};

		case '[':
			return Instruction{
				0,
				InstructionType::BeginLoop
			};

		case ']':
			return Instruction{
				0,
				InstructionType::EndLoop
			};
		}
	}

	std::vector<Instruction> optimize_instructions(const std::vector<Instruction>& instructions);
}
