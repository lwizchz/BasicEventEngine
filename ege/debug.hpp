/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of EGE.
* EGE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _EGE_DEBUG_H
#define _EGE_DEBUG_H 1

#include <sstream>

std::string debug_indent(std::string input, int amount) {
	if (input.size() > 0) {
		input.pop_back();
		std::istringstream input_stream (input);
		std::string output;
		while (!input_stream.eof()) {
			std::string tmp;
			getline(input_stream, tmp);
			tmp.append("\n");
			output.append(amount, '\t');
			output.append(tmp);
		}
		return output;
	}
	return "\n";
}

#endif // _EGE_DEBUG_H
