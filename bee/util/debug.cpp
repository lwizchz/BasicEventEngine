/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DEBUG
#define BEE_DEBUG 1

#include <sstream>

#include <SDL2/SDL.h>

#include "debug.hpp" // Include the function declarations

namespace util {

/**
* Format the given string with a certain amount of indentation and a delimiter prepended to each line.
* @param input the string to format
* @param amount the amount of tabs '\t' to prepend
* @param delimiter the string to prepend each line with
*
* @returns the formatted string
*/
std::string debug_indent(const std::string& input, int amount, const std::string& delimiter) {
	if (input.empty()) {
		return "\n";
	}

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

	return output;
}
/**
* Format the given string with a certain amount of indentation and a delimiter prepended to each line.
* @note If the function is called without a delimiter, then let it be an empty string.
* @param input the string to format
* @param amount the amount of tabs '\t' to prepend
*
* @returns the formatted string
*/
std::string debug_indent(const std::string& input, int amount) {
	return debug_indent(input, amount, "");
}

/**
* @param shader the OpenGL index of the given shader
*
* @returns the error string from when an OpenGL shader fails to compile
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

	return s;
}
/**
* @param program the OpenGL index of the given program
*
* @returns the error string from when an OpenGL program fails to link
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

	return s;
}

/**
* @returns the SDL error string as a C++ string
*/
std::string get_sdl_error() {
	return std::string(SDL_GetError());
}

}

#endif // BEE_DEBUG
