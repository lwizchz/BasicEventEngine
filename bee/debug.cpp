/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_DEBUG
#define _BEE_DEBUG 1

#include "debug.hpp" // Include the function declarations

/*
* debug_indent() - Format the given string with a certain amount of indentation and a delimiter prepended to each line
* @input: the string to format
* @amount: the amount of tabs '\t' to prepend
* @delimiter: the string to prepend each line with
*/
std::string debug_indent(const std::string& input, int amount, const std::string& delimiter) {
	if (!input.empty()) { // if the given string contains text
		//input.pop_back(); // Remove the last character which should be a newline '\n'

		std::istringstream input_stream (input); // Create a stream from the string to allow line-by-line modification
		std::string output; // Declare the return string

		while (!input_stream.eof()) { // Continue as long as the stream has not reached the end
			std::string tmp;
			getline(input_stream, tmp); // Get the next line of the stream

			tmp.append("\n"); // Re-add each newline which is removed in getline()

			// Append the delimiter, a tab '\t', and the line to the return string
			output.append(delimiter);
			output.append(amount, '\t');
			output.append(tmp);
		}
		return output; // Return the indented string
	}
	return "\n"; // Return a newline when the given string is empty
}
/*
* debug_indent() - Format the given string with a certain amount of indentation and a delimiter prepended to each line
* ! When the function is called without a delimiter, simply call it with an empty delimiter
* @input: the string to format
* @amount: the amount of tabs '\t' to prepend
*/
std::string debug_indent(const std::string& input, int amount) {
	return debug_indent(input, amount, "");
}

/*
* get_shader_error() - Return the error string from when an OpenGL shader fails to compile
* @shader: the OpenGL index of the given shader
*/
std::string get_shader_error(GLuint shader) {
	int length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length); // Get the length of the error string
	if (length <= 0) { // If the log length is not enough, then return an empty string
		return "";
	}

	char* log = new char[length]; // Allocate a new char array to store the log
	glGetShaderInfoLog(shader, length, nullptr, log); // Fetch the error string into the char array
	std::string s (log); // Convert the char array into a string in order to return it more easily
	delete[] log; // Free the memory of the char array

	return s; // Return the error string
}
/*
* get_program_error() - Return the error string from when an OpenGL program fails to link
* @program: the OpenGL index of the given program
*/
std::string get_program_error(GLuint program) {
	int length = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length); // Get the length of the error string
	if (length <= 0) { // If the log length is not enough, then return an empty string
		return "";
	}

	char* log = new char[length]; // Allocate a new char array to store the log
	glGetProgramInfoLog(program, length, nullptr, log); // Fetch the error string into the char array
	std::string s (log); // Convert the char array into a string in order to return it more easily
	delete[] log; // Free the memory of the char array

	return s; // Return the error string
}

#endif // _BEE_DEBUG
