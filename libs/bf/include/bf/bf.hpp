#pragma once

#include <cstdint>
#include <iostream>

namespace bf::opt {
	enum class InstructionType : uint8_t {
		AddPtr,
		SubPtr,
		AddVal,
		SubVal,
		Print,
		Read,
		BeginLoop,
		EndLoop
	};

	struct Instruction {
		uint8_t data;
		InstructionType type;
	};

	static_assert(sizeof(Instruction) == 2);

	inline Instruction parse_token(const char token) {
		switch (token) {
		case '>':
			return Instruction{
				1,
				InstructionType::AddPtr
			};

		case '<':
			return Instruction{
				1,
				InstructionType::SubPtr
			};

		case '+':
			return Instruction{
				1,
				InstructionType::AddVal
			};

		case '-':
			return Instruction{
				1,
				InstructionType::SubVal
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

		default:
			std::cerr << "Could not parse token '" << token << ";\n";
			return {};
		}
	}
}
