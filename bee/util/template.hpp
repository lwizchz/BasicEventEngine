/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_TEMPLATE_H
#define _BEE_UTIL_TEMPLATE_H 1

// Template-requiring functions

#include <string> // Include the required library headers
#include <vector>
#include <algorithm>
#include <sstream>
#include <map>
#include <iostream>

/* * * * * * * * * * * *
* Real number functions *
* * * * * * * * * * * * */

/*
* choose() - Return a random value from the given arguments
* @args: the arguments to choose from
*/
template <typename T, typename... Args>
T choose(Args&&... args) {
	auto t = std::make_tuple(std::forward<Args>(args)...); // Create a tuple of the given arguments
	unsigned int size = std::tuple_size<decltype(t)>::value;

	std::vector<T> arguments (size); // Convert the tuple into a vector
	arguments = {std::forward<Args>(args)...};

	return arguments[random(size)]; // Return a random element from the vector on success
}

/*
* min() - Return the minimum value of the given arguments
* @args: the arguments to evaluate
*/
template <typename T, typename... Args>
T min(Args&&... args) {
	auto t = std::make_tuple(std::forward<Args>(args)...); // Create a tuple of the given arguments
	unsigned int size = std::tuple_size<decltype(t)>::value;

	std::vector<T> arguments (size); // Convert the tuple into a vector
	arguments = {std::forward<Args>(args)...};

	std::sort(arguments.begin(), arguments.end());
	return arguments[0]; // Return the first value after sorting from least to greatest on success, i.e. the minimum
}
/*
* max() - Return the maximum value of the given arguments
* @args: the arguments to evaluate
*/
template <typename T, typename... Args>
T max(Args&&... args) {
	auto t = std::make_tuple(std::forward<Args>(args)...); // Create a tuple of the given arguments
	unsigned int size = std::tuple_size<decltype(t)>::value;

	std::vector<T> arguments (size); // Convert the tuple into a vector
	arguments = {std::forward<Args>(args)...};

	std::sort(arguments.begin(), arguments.end());
	return arguments[size-1]; // Return the last value after sorting from least to greatest on success, i.e. the maximum
}
/*
* mean() - Return the mean average of the given arguments
* @args: the arguments to evaluate
*/
template <typename T, typename... Args>
T mean(Args&&... args) {
	auto t = std::make_tuple(std::forward<Args>(args)...); // Create a tuple of the given arguments
	unsigned int size = std::tuple_size<decltype(t)>::value;

	std::vector<T> arguments (size); // Convert the tuple to a vector
	arguments = {std::forward<Args>(args)...};

	T sum = 0;
	for (auto a : arguments) { // Iterate over the vector and sum up each element
		sum += a;
	}
	return sum/size; // Return the mean on success
}
/*
* median() - Return the median average of the given arguments
* @args: the arguments to evaluate
*/
template <typename T, typename... Args>
T median(Args&&... args) {
	auto t = std::make_tuple(std::forward<Args>(args)...); // Create a tuple of the given arguments
	unsigned int size = std::tuple_size<decltype(t)>::value;

	std::vector<T> arguments (size); // Convert the tuple into a vector
	arguments = {std::forward<Args>(args)...};

	std::sort(arguments.begin(), arguments.end());
	if (size % 2 == 1) { // If there is an odd number of elements
		return arguments[(int)(size/2)]; // Return the middle element on success
	} else {
		return (arguments[size/2-1] + arguments[size/2])/2; // Return the mean of the two middle elements on success
	}
}

/* * * * * * * * * * * * * *
* String handling functions *
* * * * * * * * * * * * * * */

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

/* * * * * * * * * * * *
* Networking functions *
* * * * * * * * * * * * */

/*
* network_map_encode() - Return an array of Uint8's which represent a map
* ! Note that the first element in the array is the size of the array, thus the maximum length is 255
* @m: the map to encode
*/
template <typename A, typename B>
Uint8* network_map_encode(std::map<A,B> m) {
	std::stringstream s ("1"); // Create a string stream for temporary encoding
	for (auto& e : m) { // Iterate over the map and put the keys and values into the newline-separated stream
		s << e.first << " = " << e.second << "\n";
	}

	Uint8* d = orda(s.str()); // Convert the string into an array of Uint8's
	d[0] = s.str().length(); // Set the first element of the array to the array size
	return d; // Return the array on success
}
/*
* network_map_decode() - Decode the given array of Uint8's into the given map
* @data: the array to decode
* @mv: the map to store the values in
*/
template <typename A, typename B>
int network_map_decode(Uint8* data, std::map<A,B>* mv) {
	std::map<A,B> m; // Declare a temporary map
	std::string datastr = chra(data); // Convert the array to a string
	if (!datastr.empty()) { // If the string is not empty
		std::istringstream data_stream (datastr); // Create a stream from the string

		while (!data_stream.eof()) { // Operate on each line of the stream
			std::string tmp;
			getline(data_stream, tmp); // Fetch the line from the stream into a temporary string

			if ((tmp.empty())||(tmp[0] == '#')) { // If the line is empty or begins with a comment character, then go to the next
				continue;
			}

			std::string ks = tmp.substr(0, tmp.find(" = ")); // Get the key and value pair
			ks = trim(ks);
			std::string vs = tmp.substr(tmp.find(" = ")+3);
			vs = trim(vs);

			std::istringstream k (ks), v (vs);
			A key;
			B value;
			k >> key;
			v >> value;

			m.emplace(key, value); // Add the pair to the map
		}

	}
	*mv = m; // Set the given map to the temporary map on success
	return 0; // Return 0 on success
}

/* * * * * * * * * * *
* Printing functions *
* * * * * * * * * * * */

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

#endif // _BEE_UTIL_TEMPLATE_H
