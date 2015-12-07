/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
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

#endif // _BEE_DEBUG
