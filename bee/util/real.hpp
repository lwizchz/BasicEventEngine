/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_REAL_H
#define BEE_UTIL_REAL_H 1

#define GLM_FORCE_RADIANS

#include <string> // Include the required library headers
#include <vector>

#include <glm/glm.hpp> // Include the required headers for vector handling
#include <btBulletDynamicsCommon.h>

#define PI 3.141592653589793238462643383279502884197 // 40 digits of pi, sufficient to calculate the circumference of the observable universe to the width of one atom

namespace util {

template <typename T>
extern int sign(T);
template <typename T>
extern T sqr(T);
double logn(double, double);
double degtorad(double);
double radtodeg(double);
double opposite_angle(double);
double absolute_angle(double);

double direction_of(double, double, double, double);
btVector3 direction_of(double, double, double, double, double, double);
double dist_sqr(double, double, double, double, double, double);
double dist_sqr(double, double, double, double);
double distance(double, double, double, double, double, double);
double distance(double, double, double, double);
std::pair<double,double> coord_approach(double, double, double, double, double, double);
glm::vec3 bt_to_glm3(const btVector3&);
btVector3 glm_to_bt3(const glm::vec3&);

template <typename T>
extern bool is_between(T, T, T);
template <typename T>
extern bool is_angle_between(T, T, T);
template <typename T>
extern T fit_bounds(T, T, T);
template <typename T>
extern T qmod(T, unsigned int);

namespace random {
	unsigned int get_range(unsigned int, unsigned int);
	unsigned int get(int);
	unsigned int get_seed();
	unsigned int set_seed(unsigned int);
	unsigned int reset_seed();
	unsigned int randomize();
}

namespace checksum {
	namespace internal {
		unsigned int reflect(unsigned int, const char);
		unsigned int table(size_t);
	}
	unsigned int get(const std::vector<unsigned char>&);
	bool verify(const std::vector<unsigned char>&, unsigned int);
}

}

#endif // BEE_UTIL_REAL_H
