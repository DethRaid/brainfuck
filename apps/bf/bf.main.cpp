//! \brief Brainfuck interpreter
//!
//! This file provides a command-line interface to the Brainfuck interpreter

#include "iostream"

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

		}

		std::cout << "Hello World!\n";

		return 0;

	}
	catch (...) {
		return 1;
	}
}