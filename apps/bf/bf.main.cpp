//! \brief Brainfuck interpreter
//!
//! This file provides a command-line interface to the Brainfuck interpreter

#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <cstring>
#include <string>
#include <stack>

#include <bf/bf.hpp>
#include <vector>

// from https://stackoverflow.com/questions/865668/how-to-parse-command-line-arguments-in-c, edited for me
class InputParser {
public:
	InputParser(int& argc, char** argv) {
		for (int i = 1; i < argc; ++i) {
			tokens.emplace_back(argv[i]);
		}
	}

	/// @author iain
	[[nodiscard]] const std::string& get_cmd_option(const std::string& option) const {
		auto itr = std::find(tokens.begin(), tokens.end(), option);
		if (itr != tokens.end() && ++itr != tokens.end()) {
			return *(itr + 2);	// Advance the iterator a little so we just get the value of the option, not the option itself
		}
		static const std::string EMPTY_STRING;
		return EMPTY_STRING;
	}

	/// @author iain
	[[nodiscard]] bool cmd_option_exists(const std::string& option) const {
		return std::find(tokens.begin(), tokens.end(), option) != tokens.end();
	}

private:
	std::vector<std::string> tokens;
};

namespace bf {
	/*
	 * Configuration parameters
	 */

	 /*!
	  * \brief Minimum size of the tape that bf provides to the program
	  *
	  * Must be at least 30,000 to conform to the Brainfuck spec
	  */
	constexpr uint64_t MIN_TAPE_SIZE = 32768u;

	/*
	 * Error codes that bf might return
	 */

	 //! \brief Unknown error because something threw an exception
	constexpr auto ERR_UNKNOWN = -0x01;

	//! \brief You provided bad arguments to bf
	constexpr auto ERR_WRONG_ARGUMENTS = -0x10;

	//! \brief The input brainfuck program is malformed
	constexpr auto ERR_INVALID_PROGRAM = -0x11;

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
	[[nodiscard]] bool interpret(const std::string& token_stream, const uint64_t& tape_size, std::istream& bstdin, std::ostream& bstdout) {
		const auto real_tape_size = std::min(tape_size, MIN_TAPE_SIZE);
		const auto tape = std::make_unique<char*>(new char[real_tape_size]);
		std::memset(*tape, 0, real_tape_size);

		auto* tape_ptr = *tape;
		auto tape_idx = 0u;

		auto read_idx = 0u;
		auto loop_start_idx = std::stack<uint64_t>{};

		while (read_idx < token_stream.size()) {
			const char token = token_stream[read_idx];

			// Intentionally no `default` case because in Brainfuck anything that isn't a known token is ignored
			// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
			switch (token) {  // NOLINT(hicpp-multiway-paths-covered)
			case '>':
				tape_idx = static_cast<uint64_t>(tape_idx + 1) % MIN_TAPE_SIZE;
				break;

			case '<':
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
					loop_start_idx.push(read_idx);

				}
				else {
					// Skip to the ]

					auto loop_depth = 1u;
					auto skip_token = token;	// Initialize it to get the type

					do {
						read_idx++;

						skip_token = token_stream[read_idx];

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
				// We increment the read idx after this line, so I have to decrement it here to get correct results
				read_idx = loop_start_idx.top() - 1;
				loop_start_idx.pop();
				break;
			}

			read_idx++;
		}

		return true;
	}

	[[nodiscard]] bool interpret(const std::vector<opt::Instruction>& instructions, const uint64_t& tape_size, std::istream& bstdin, std::ostream& bstdout) {
		const auto real_tape_size = std::min(tape_size, MIN_TAPE_SIZE);
		const auto tape = std::make_unique<char*>(new char[real_tape_size]);
		std::memset(*tape, 0, real_tape_size);

		auto* tape_ptr = *tape;
		auto tape_idx = 0u;

		auto read_idx = 0u;
		auto loop_start_idx = std::stack<uint64_t>{};

		while (read_idx < instructions.size()) {
			const auto& instruction = instructions[read_idx];

			// Intentionally no `default` case because in Brainfuck anything that isn't a known token is ignored
			// ReSharper disable once CppDefaultCaseNotHandledInSwitchStatement
			switch (instruction.type) {  // NOLINT(hicpp-multiway-paths-covered)
			case opt::InstructionType::AddPtr:
				tape_idx = static_cast<uint64_t>(instruction.data + tape_idx) % MIN_TAPE_SIZE;
				break;

			case opt::InstructionType::SubPtr:
				tape_idx = (tape_idx - instruction.data) % MIN_TAPE_SIZE;
				break;

			case opt::InstructionType::AddVal:
				tape_ptr[tape_idx] += instruction.data;
				break;

			case opt::InstructionType::SubVal:
				tape_ptr[tape_idx] -= instruction.data;
				break;

			case opt::InstructionType::Print:
				bstdout << tape_ptr[tape_idx];
				break;

			case opt::InstructionType::Read:
				bstdin >> tape_ptr[tape_idx];
				break;

			case opt::InstructionType::BeginLoop:
			{
				if (tape_ptr[tape_idx]) {
					loop_start_idx.push(read_idx);

				}
				else {
					// Skip to the ]

					auto loop_depth = 1u;
					auto skip_instruction = instruction;	// Initialize it to get the type

					do {
						read_idx++;

						skip_instruction = instructions[read_idx];

						if (skip_instruction.type == opt::InstructionType::EndLoop) {
							loop_depth--;
						}
						else if (skip_instruction.type == opt::InstructionType::BeginLoop) {
							loop_depth++;
						}
					} while (loop_depth != 0);
				}
			}
			break;

			case opt::InstructionType::EndLoop:
				// We increment the read idx after this line, so I have to decrement it here to get correct results
				read_idx = loop_start_idx.top() - 1;
				loop_start_idx.pop();
				break;
			}

			read_idx++;
		}

		return true;
	}

	bool is_valid(const char token) {
		return token == '<' || token == '>' || token == '+' || token == '-' || token == '[' || token == ']' || token == ',' || token == '.';
	}

	void print_help() {
		std::cerr << "bf\n\nInterprets a Brainfuck program from standard inputs\n\nbf <filename>\n\nInterprets a Brainfuck program from a file\n\nParameters:\n\n* filename: Filepath to the file to interpret Brainfuck from\n";
	}
}

/*!
 * \brief Entry point for my Brainfuck interpreter
 *
 * Usage: `bf <filename> <opts>`
 *
 * Parameters:
 * - filename: Name of the brainfuck file to interpret. May have any extension of reside on any drive accessible
 * through the current filesystem
 *
 * Options:
 * -t<num>: Sets the size of the tape that the interpreter will use. If `num` is less than 32,768, then the tape will be 32,768 cells long
 *
 * -o: Tells the interpreter perform optimizations on the program, such as combining multiple increments into a single add
 *
 */
int main(int argc, char** argv) {
	try {
		const InputParser input(argc, argv);

		const auto tape_size = [&]() -> uint64_t {
			if (input.cmd_option_exists("-t")) {				
				const auto& tape_size_str = input.get_cmd_option("-t");

				if(!tape_size_str.empty() && std::isdigit(tape_size_str[0])) {
					return std::stoll(tape_size_str);
				}
				else {
					std::cout << "You must pass in a positive number for the tape size\n";
					return bf::MIN_TAPE_SIZE;
				}
			}
			else {
				return bf::MIN_TAPE_SIZE;
			}
		}();

		const auto optimize = input.cmd_option_exists("-o");

		auto* token_stream = [&]() -> std::istream* {
			if (argc >= 2) {
				return new std::ifstream(argv[1]);

			}
			else {
				return &std::cin;
			}
		}();

		// TODO: Read a few tokens from the stream, send them off to the optimizer, which then send optimized tokens to the
		// runtime, which actually runs the program

		// TODO: Optimize token chunks on a thread pool. We'll need plenty of synchronization, of course

		// TODO: Use loops as the boundaries of the token chunks for better instruction cache performance

		std::string tokens = [&] {
			if (token_stream->good()) {
				std::string _tokens;

				token_stream->seekg(0, std::ios::end);
				_tokens.reserve(token_stream->tellg());
				token_stream->seekg(0, std::ios::beg);

				_tokens.assign((std::istreambuf_iterator<char>(*token_stream)), std::istreambuf_iterator<char>());

				return _tokens;
			}
			else {
				std::cerr << "Could not open Brainfuck file " << argv[1] << "\n";

				return std::string{};
			}
		}();

		const bool valid_program = [&] {
			if (optimize) {
				std::vector<bf::opt::Instruction> instructions;
				instructions.reserve(tokens.size());

				// I'd like a proper monad that isn't stupidly verbose but at least there's something?
				const auto& remove_itr = std::remove_if(tokens.begin(), tokens.end(), [](const auto token) { return !bf::is_valid(token); });
				std::transform(tokens.begin(), remove_itr, std::back_inserter(instructions), bf::opt::parse_token);

				const auto& optimized_instructions = optimize_instructions(instructions);

				return bf::interpret(optimized_instructions, tape_size, std::cin, std::cout);
			}
			else {
				return bf::interpret(tokens, tape_size, std::cin, std::cout);
			}
		}();

		if (!valid_program) {
			std::cout << "Program " << argv[0] << " is invalid";

			return bf::ERR_INVALID_PROGRAM;
		}

		return 0;

	}
	catch (...) {
		return bf::ERR_UNKNOWN;
	}
}