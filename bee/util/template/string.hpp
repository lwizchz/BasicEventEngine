/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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
#include <sstream>
#include <map>

#include "../string.hpp"

namespace util {

/**
* Convert a vector into a serialized string.
* @param v the vector to serialize
* @param should_pretty_print whether the vector shold be printed in a human readable format
*
* @returns the serialized string
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

	std::ostringstream ss;
	ss << "[" << pp_newline;

	size_t i = 0;
	for (auto& e : v) {
		ss << pp_indent << e;

		if (i < v.size()-1) {
			ss << pp_field_sep;
		}
		ss << pp_newline;
		++i;
	}

	ss << "]";

	return ss.str();
}
/**
* Convert a serialized string into a vector.
* @param s the string to deserialize
* @param v the vector to store the data in
*
* @retval 0 success
* @retval 1 invalid input string
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
		e = trim(e);
		if (e.empty()) {
			break;
		}

		std::istringstream es (e);
		A element;
		es >> element;

		v->push_back(element); // Add the element to the vector
	}

	return 0;
}

/**
* Convert a map into a serialized string.
* @param m the map to serialize
* @param should_pretty_print whether the map should be printed in a human readable format
*
* @returns the serialized string
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

	std::ostringstream ss;
	ss << "{" << pp_newline;

	size_t i = 0;
	for (auto& e : m) {
		ss << pp_indent << e.first << pp_set << e.second;

		if (i < m.size()-1) {
			ss << pp_field_sep;
		}
		ss << pp_newline;
		++i;
	}

	ss << "}";

	return ss.str();
}
/**
* Convert a serialized string into a map.
* @param s the string to deserialize
* @param m the map to store the data in
*
* @retval 0 success
* @retval 1 invalid input string
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
		
		if ((ks.empty())||(vs.empty())) {
			break;
		}

		std::istringstream k (ks);
		A key;
		k >> key;

		std::istringstream v (vs);
		B value;
		v >> value;

		m->erase(key);
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

}

#endif // BEE_UTIL_TEMPLATE_STRING_H
