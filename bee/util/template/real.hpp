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

#include <algorithm>

#include "../real.hpp"

/*
* choose() - Return a random value from the given arguments
* @args: the arguments to choose from
*/
template <typename T>
T choose(std::initializer_list<T> args) {
	size_t index = random(args.size());
	size_t i = 0;
	for (auto& e : args) {
		if (i == index) {
			return e;
		}
	}
	return *args.begin();
}

/*
* min() - Return the minimum value of the given arguments
* @args: the arguments to evaluate
*/
template <typename T>
T min(std::initializer_list<T> args) {
	T m = *args.begin();
	for (auto& e : args) {
		if (e < m) {
			m = e;
		}
	}
	return m;
}
/*
* max() - Return the maximum value of the given arguments
* @args: the arguments to evaluate
*/
template <typename T>
T max(std::initializer_list<T> args) {
	T m = *args.begin();
	for (auto& e : args) {
		if (e > m) {
			m = e;
		}
	}
	return m;
}
/*
* mean() - Return the mean average of the given arguments
* @args: the arguments to evaluate
*/
template <typename T>
T mean(std::initializer_list<T> args) {
	T sum = T();
	for (auto& e : args) {
		sum += e;
	}
	return sum/args.size();
}
/*
* median() - Return the median average of the given arguments
* @args: the arguments to evaluate
*/
template <typename T>
T median(std::initializer_list<T> args) {
	std::vector<T> arguments (args);
	std::sort(arguments.begin(), arguments.end());

	size_t s = args.size();
	if (s % 2 == 1) {
		return arguments[s/2];
	} else {
		return (arguments[s/2-1] + arguments[s/2])/2;
	}
}

#endif // BEE_UTIL_TEMPLATE_REAL_H
