//! \brief Brainfuck interpreter
//!
//! This file provides a command-line interface to the Brainfuck interpreter

#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <cstring>

/*
 * Configuration parameters
 */

 /*!
  * \brief Minimum size of the tape that bf provides to the program
  *
  * Must be at least 30,000 to conform to the Brainfuck spec
  */
constexpr auto MIN_TAPE_SIZE = 32768u;

/*
 * Error codes that bf might return
 */

 //! \brief Unknown error because something threw an exception
constexpr auto BF_ERR_UNKNOWN = -0x01;

//! \brief You provided bad arguments to br
constexpr auto BF_ERR_WRONG_ARGUMENTS = -0x10;
constexpr auto BF_ERR_INVALID_PROGRAM = -0x11;

/*!
 * \brief Interprets a stream of Brainfuck instructions
 *
 * This interpreting routine has a minimum tape size of 32768 bytes. Each byte is 8 bits
 *
 * \param token_stream The stream of Brainfuck instructions to interpret
 * \param tape_size The size of the tape that Brainfuck program will use. If this size is less than 32768, your program will be given a 32768-byte long tape
 * \param bstdin Standard input stream that the program will read from
 * \param bstdout Standard output stream that the program will write to
 */
bool interpret(std::istream& token_stream, const uint32_t& tape_size, std::istream& bstdin, std::ostream& bstdout) {
	const uint32_t real_tape_size = std::min(tape_size, MIN_TAPE_SIZE);
	const auto tape = std::make_unique<char*>(new char[real_tape_size]);
	std::memset(*tape, 0, real_tape_size);

	auto* tape_ptr = *tape;
	auto tape_idx = 0;

	auto loop_start = token_stream.tellg();

	while (token_stream.good()) {
		const auto token = token_stream.get();

		// Intentionally no `default` case because in Brainfuck anything that isn't a known token is ignored
		// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
		switch (token) {  // NOLINT(hicpp-multiway-paths-covered)
		case '<':
			tape_idx = (tape_idx + 1) % MIN_TAPE_SIZE;
			break;

		case '>':
			tape_idx = (tape_idx - 1) % MIN_TAPE_SIZE;
			break;

		case '+':
			++tape_ptr[tape_idx];
			break;

		case '-':
			--tape_ptr[tape_idx];
			break;

		case '.':
			bstdout << tape_ptr[tape_idx];
			break;

		case ',':
			bstdin >> tape_ptr[tape_idx];
			break;

		case '[':
		{
			if (tape_ptr[tape_idx]) {
				loop_start = token_stream.tellg();

			}
			else {
				auto loop_depth = 1u;

				// Skip to the ]
				auto skip_token = token;	// Initialize it to get the type
				do {
					skip_token = token_stream.get();

					if (skip_token == ']') {
						loop_depth--;
					}
					else if (skip_token == '[') {
						loop_depth++;
					}
				} while (loop_depth != 0);
			}
		}
		break;

		case ']':
			token_stream.seekg(loop_start);
			break;
		}
	}

	return true;
}

void print_help() {
	std::cerr << "bf\n\nInterprets a Brainfuck program from standard inputs\n\nbf <filename>\n\nInterprets a Brainfuck program from a file\n\nParameters:\n\n* filename: Filepath to the file to interpret Brainfuck from\n";
}

/*!
 * \brief Entry point for my Brainfuck interpreter
 *
 * Usage: `bf <filename>`
 *
 * Parameters:
 * - filename: Name of the brainfuck file to interpret. May have any extension of reside on any drive accessible
 * through the current filesystem
 */
int main(int argc, const char** argv) {
	try {
		if (argc > 3) {
			std::cerr << "Incorrect arguments. Printing help page...\n";
			print_help();
			return BF_ERR_WRONG_ARGUMENTS;
		}

		auto* token_stream = [&]() -> std::istream* {
			if (argc == 2) {
				return new std::ifstream(argv[1]);

			}
			else {
				return &std::cin;
			}
		}();

		const auto valid_program = interpret(*token_stream, MIN_TAPE_SIZE, std::cin, std::cout);
		if (!valid_program) {
			std::cout << "Program " << argv[0] << " is invalid";

			return BF_ERR_INVALID_PROGRAM;
		}

		return 0;

	}
	catch (...) {
		return BF_ERR_UNKNOWN;
	}
}