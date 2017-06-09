/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_REAL_H
#define BEE_UTIL_REAL_H 1

#include <string> // Include the required library headers

#include <glm/glm.hpp> // Include the required headers for vector handling
#include <btBulletDynamicsCommon.h>

#define PI 3.141592653589793238462643383279502884197 // 40 digits of pi, sufficient to calculate the circumference of the observable universe to the width of one atom
#define DEFAULT_RANDOM_SEED 1 // A terrible default seed which never actually is used

unsigned int random_internal(unsigned int, unsigned int, unsigned int);
unsigned int random(int);
unsigned int random_range(unsigned int, unsigned int);
unsigned int random_get_seed();
unsigned int random_set_seed(unsigned int);
unsigned int random_reset_seed();
unsigned int randomize();
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
double distance(double, double, double);
double distance(double, double);
std::pair<double,double> coord_approach(double, double, double, double, double, double);
glm::vec3 bt_to_glm3(const btVector3&);
btVector3 glm_to_bt3(const glm::vec3&);
template <typename T>
extern T dot_product(T, T, T, T);
template <typename T>
extern T dot_product(const std::pair<T,T>&, const std::pair<T,T>&);
template <typename T>
extern bool is_between(T, T, T);
template <typename T>
extern bool is_angle_between(T, T, T);
template <typename T>
extern T fit_bounds(T, T, T);

#endif // BEE_UTIL_REAL_H
