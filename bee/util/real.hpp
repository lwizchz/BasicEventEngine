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

unsigned int random(int x) {
        if (x == 0) {
                return 0;
        }
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

template <typename T>
int sign(T x) {
        if (x > 0) {
                return 1;
        } else if (x < 0) {
                return -1;
        }
        return 0;
}
template int sign<int>(int);
template int sign<double>(double);

template <typename T>
T sqr(T x) {
        return x*x;
}
template int sqr<int>(int);
template double sqr<double>(double);

double logn(double n, double x) {
        return log(x)/log(n);
}
double degtorad(double a) {
	return a*PI/180.0;
}
double radtodeg(double a) {
	return a*180.0/PI;
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
template int dot_product<int>(int, int, int, int);
template double dot_product<double>(double, double, double, double);

template <typename T>
T dot_product(std::pair<T,T> p1, std::pair<T,T> p2) {
        return p1.first*p2.first + p1.second*p2.second;
}
template int dot_product<int>(std::pair<int,int>, std::pair<int,int>);
template double dot_product<double>(std::pair<double,double>, std::pair<double,double>);

template <typename T>
bool is_between(T x, T a, T b) {
        if ((x > a)&&(x < b)) {
                return true;
        }
        return false;
}
template bool is_between<int>(int, int, int);
template bool is_between<double>(double, double, double);

template <typename T>
bool is_angle_between(T x, T a, T b) {
        return is_between(x, (T)0, b) || is_between(x, a, (T)360);
}
template bool is_angle_between<int>(int, int, int);
template bool is_angle_between<double>(double, double, double);

#endif // _BEE_UTIL_REAL_H
