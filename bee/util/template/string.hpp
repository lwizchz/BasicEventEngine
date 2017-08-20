/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_TEMPLATE_STRING_H
#define BEE_UTIL_TEMPLATE_STRING_H 1

// String template-requiring functions

#include <string> // Include the required library headers
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include <iostream>

/*
* vector_serialize() - Convert a vector into a serialized string
* @v: the vector to serialize
* @should_pretty_print: whether the vector shold be printed in a human readable format
*/
template <typename A>
std::string vector_serialize(const std::vector<A>& v, bool should_pretty_print) {
	std::string pp_field_sep = ", ";
	std::string pp_indent = "\t";
	std::string pp_newline = "\n";
	if (!should_pretty_print) {
		pp_field_sep = ",";
		pp_indent.clear();
		pp_newline.clear();
	}

	std::stringstream s;
	s << "[" << pp_newline;

	size_t i = 0;
	for (auto& e : v) {
		s << pp_indent << e;

		if (i < v.size()-1) {
			s << pp_field_sep;
		}
		s << pp_newline;
		++i;
	}

	s << "]";

	return s.str();
}
/*
* vector_deserialize() - Convert a serialized string into a vector
* @s: the string to deserialize
* @v: the vector to store the data in
*/
template <typename A>
int vector_deserialize(const std::string& s, std::vector<A>* v) {
	std::string ns = trim(s);
	if (ns.length() < 2) {
		return 1;
	}
	ns = ns.substr(1, ns.length()-2); // Remove front and back brackets

	std::vector<std::string> elements = splitv(ns, ',', true);
	for (auto& e : elements) {
		std::istringstream es (e);
		A element;
		es >> element;

		v->push_back(element); // Add the element to the vector
	}

	return 0;
}

/*
* map_serialize() - Convert a map into a serialized string
* @m: the map to serialize
* @should_pretty_print: whether the map should be printed in a human readable format
*/
template <typename A, typename B>
std::string map_serialize(const std::map<A,B>& m, bool should_pretty_print) {
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
template <typename A, typename B>
int map_deserialize(const std::string& s, std::map<A,B>* m) {
	std::string ns = trim(s);
	if (ns.length() < 2) {
		return 1;
	}
	ns = ns.substr(1, ns.length()-2); // Remove front and back brackets

	std::vector<std::string> elements = splitv(ns, ',', true);
	for (auto& e : elements) {
		std::string ks = e.substr(0, e.find(":")); // Get the key and value pair
		ks = trim(ks);
		std::string vs = e.substr(e.find(":")+1);
		vs = trim(vs);

		std::istringstream k (ks);
		A key;
		k >> key;

		std::istringstream v (vs);
		B value;
		v >> value;

		if (m->find(key) != m->end()) {
			m->erase(key);
		}
		m->emplace(key, value); // Add the pair to the map
	}

	return 0;
}

template <typename A>
std::ostream& operator<<(std::ostream& os, const std::vector<A>& v)
{
	os << vector_serialize(v, true);
	return os;
}
template <typename A, typename B>
std::ostream& operator<<(std::ostream& os, const std::map<A,B>& m)
{
	os << map_serialize(m, true);
	return os;
}

#endif // BEE_UTIL_TEMPLATE_STRING_H
