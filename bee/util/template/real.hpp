/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_TEMPLATE_REAL_H
#define BEE_UTIL_TEMPLATE_REAL_H 1

// Real template-requiring functions

#include <string> // Include the required library headers
#include <vector>
#include <algorithm>
#include <sstream>
#include <tuple>

#include "../real.hpp"

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
		//return arguments[static_cast<int>(size/2)]; // Return the middle element on success
		return arguments[size/2]; // Return the middle element on success
	} else {
		return (arguments[size/2-1] + arguments[size/2])/2; // Return the mean of the two middle elements on success
	}
}

#endif // BEE_UTIL_TEMPLATE_REAL_H
