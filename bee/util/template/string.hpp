/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_TEMPLATE_STRING_H
#define _BEE_UTIL_TEMPLATE_STRING_H 1

// String template-requiring functions

#include <string> // Include the required library headers
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include <iostream>

/*
* map_serialize() - Convert a map into a serialized string
* @m: the map to serialize
* @should_pretty_print: whether the map should be printed in a human readable format
*/
template <typename A>
std::string map_serialize(std::map<std::string,A> m, bool should_pretty_print) {
	std::string pp_set = ": ";
	std::string pp_field_sep = ", ";
	std::string pp_indent = "\t";
	std::string pp_newline = "\n";
	if (!should_pretty_print) {
		pp_set = ":";
		pp_field_sep = ",";
		pp_indent.clear();
		pp_newline.clear();
	}

	std::stringstream s;
	s << "{" << pp_newline;

	size_t i = 0;
	for (auto& e : m) {
		s << pp_indent << e.first << pp_set << e.second;

		if (i < m.size()-1) {
			s << pp_field_sep;
		}
		s << pp_newline;
		++i;
	}

	s << "}";

	return s.str();
}
/*
* map_deserialize() - Convert a serialized string into a map
* @s: the string to deserialize
* @m: the map to store the data in
*/
template <typename A>
int map_deserialize(std::string s, std::map<std::string,A>* mv) {
	s = trim(s);
	s = s.substr(1, s.length()-2); // Remove front and back brackets

	std::map<std::string,A> m;
	std::vector<std::string> v = splitv(s, ',', true);
	for (auto& e : v) {
		std::string ks = e.substr(0, e.find(":")); // Get the key and value pair
		ks = trim(ks);
		std::string vs = e.substr(e.find(":")+1);
		vs = trim(vs);

		std::istringstream v (vs);
		A value;
		v >> value;

		m.emplace(ks, value); // Add the pair to the map
	}

	*mv = m;
	return 0;
}

/*
* print_map() - Format and print a map's key-value pairs on standard output
* @m: the map to print
*/
template <typename A, typename B>
int print_map(std::map<A,B> m) {
	std::cout << "{\n"; // Enclose the elements in brackets
	for (auto& e : m) {
		std::cout << "\t" << e.first << ": " << e.second << "\n"; // Print each element on a separate line in the following format: "      key: value"
	}
	std::cout << "}\n";
	return 0; // Return 0 on success
}

#endif // _BEE_UTIL_TEMPLATE_STRING_H
