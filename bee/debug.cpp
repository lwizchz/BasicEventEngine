/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_DEBUG
#define _BEE_DEBUG 1

#include "debug.hpp"

std::string debug_indent(std::string input, int amount, std::string delimiter) {
	if (input.size() > 0) {
		input.pop_back();
		std::istringstream input_stream (input);
		std::string output;
		while (!input_stream.eof()) {
			std::string tmp;
			getline(input_stream, tmp);
			tmp.append("\n");
			output.append(delimiter);
			output.append(amount, '\t');
			output.append(tmp);
		}
		return output;
	}
	return "\n";
}
std::string debug_indent(std::string input, int amount) {
	return debug_indent(input, amount, "");
}

std::string get_shader_error(GLuint shader) {
	int length = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

	char* log = new char[length];
	glGetShaderInfoLog(shader, length, NULL, log);
	std::string s (log);
	delete[] log;

	return s;
}
std::string get_program_error(GLuint program) {
	int length = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

	char* log = new char[length];
	glGetProgramInfoLog(program, length, NULL, log);
	std::string s (log);
	delete[] log;

	return s;
}

#endif // _BEE_DEBUG
