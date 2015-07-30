/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_REAL_H
#define _BEE_UTIL_REAL_H 1

// Real number functions

#include <string>
#include <random>
#include <time.h>
#include <tuple>
#include <vector>
#include <algorithm>

#define DEFAULT_RANDOM_SEED 1

unsigned int random_internal(unsigned int new_seed, unsigned int minimum, unsigned int maximum) {
        static unsigned int seed = DEFAULT_RANDOM_SEED;
        static std::mt19937 engine;

        if (seed == DEFAULT_RANDOM_SEED) {
                if (new_seed == DEFAULT_RANDOM_SEED) {
                        seed = time(NULL);
                } else {
                        seed = new_seed;
                }
                engine.seed(seed);
        }
        if (new_seed != DEFAULT_RANDOM_SEED) {
                seed = new_seed;
                engine.seed(seed);
        }

        if (minimum == 0) {
                if (maximum == 0) {
                        return engine();
                }
                return engine() % maximum;
        } else {
                if (maximum <= minimum) {
                        return seed;
                }
                return (engine() % (maximum - minimum)) + minimum;
        }
}

unsigned int random(unsigned int x) {
        return random_internal(DEFAULT_RANDOM_SEED, 0, x);
}
unsigned int random_range(unsigned int x1, unsigned int x2) {
        return random_internal(DEFAULT_RANDOM_SEED, x1, x2);
}
unsigned random_get_seed() {
        return random_internal(DEFAULT_RANDOM_SEED, 1, 0);
}
unsigned random_set_seed(unsigned int new_seed) {
        return random_internal(new_seed, 1, 0);
}
unsigned int random_reset_seed() {
        return random_set_seed(time(NULL));
}
unsigned int randomize() {
        return random_set_seed(random_internal(DEFAULT_RANDOM_SEED, 0, 0));
}
template <typename T, typename... Args>
T choose(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        return arguments[random(size)];
}

template <typename T>
int sign(T x) {
        if (x > 0) {
                return 1;
        } else if (x < 0) {
                return -1;
        }
        return 0;
}
template <typename T>
T sqr(T x) {
        return x*x;
}
float logn(float n, float x) {
        return log(x)/log(n);
}
double degtorad(double a) {
	return a*PI/180.0;
}
double radtodeg(double a) {
	return a*180.0/PI;
}

template <typename T, typename... Args>
T min(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        std::sort(arguments.begin(), arguments.end());
        return arguments[0];
}
template <typename T, typename... Args>
T max(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        std::sort(arguments.begin(), arguments.end());
        return arguments[size-1];
}
template <typename T, typename... Args>
T mean(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        T sum = 0;
        for (auto a : arguments) {
                sum += a;
        }
        return sum/size;
}
template <typename T, typename... Args>
T median(Args&&... args) {
        auto t = std::make_tuple(std::forward<Args>(args)...);
        unsigned int size = std::tuple_size<decltype(t)>::value;
        std::vector<T> arguments (size);
        arguments = {std::forward<Args>(args)...};
        std::sort(arguments.begin(), arguments.end());
        if (size % 2 == 1) {
                return arguments[(int)size/2+1];
        } else {
                return (arguments[size/2-1] + arguments[size/2])/2;
        }
}

double direction_of(double x1, double y1, double x2, double y2) {
	double dx = x2-x1;
	double dy = y2-y1;

	if (dx == 0.0) {
		if (dy >= 0.0) {
			return 0.0;
		} else {
			return 180.0;
		}
	}

	return 90.0 + radtodeg(atan2(dy, dx));
}
double distance(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

template <typename T>
T dot_product(T x1, T y1, T x2, T y2) {
        return x1*x2 + y1*y2;
}
template <typename T>
T dot_product(std::pair<T,T> p1, std::pair<T,T> p2) {
        return p1.first*p2.first + p1.second*p2.second;
}

#endif // _BEE_UTIL_REAL_H
