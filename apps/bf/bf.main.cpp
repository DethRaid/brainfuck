//! \brief Brainfuck interpreter
//!
//! This file provides a command-line interface to the Brainfuck interpreter

#include <iostream>

#include "error_codes.hpp"

/*!
 * \brief Entry point for my Brainfuck interpreter
 *
 * Usage: `bf <filename>`
 *
 * Parameters:
 * - filename: Name of the brainfuck file to interpret. May have any extension of reside on any drive accessible
 * through the current filesystem
 */
int main(const char** argc, const int argv) {
	try {
		if(argv != 1) {
			std::cerr << "bf only accepts a single argument: the name of the file to interpret\n";
			return BF_ERR_TOO_MANY_ARGUMENTS;
		}

		std::cout << "Hello World!\n";

		return 0;

	} catch(...) {
		return BF_ERR_UNKNOWN;
	}
}