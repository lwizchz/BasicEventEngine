/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_REAL_H
#define _BEE_UTIL_REAL_H 1

// Real number functions

#include <string> // Include the necessary library headers
#include <random>
#include <time.h>
#include <tuple>

#define DEFAULT_RANDOM_SEED 1 // A terrible default seed which never actually is used

/*
* random_internal() - Return a random value seeded by the given seed and bounded by the given min and max
* ! This function should probably never be called directly by the user because there are wrapper functions for it
* ! This function contains two static variables to ensure that the engine stays seeded with the correct value
* @new_seed: the integer to seed the random number engine with
* @minimum: the minimum value to return
* @maximum: the maximum value to return
*/
unsigned int random_internal(unsigned int new_seed, unsigned int minimum, unsigned int maximum) {
        static unsigned int seed = DEFAULT_RANDOM_SEED; // Define the seed for the random number engine if it hasn't been defined yet
        static std::mt19937 engine; // Declare the random number engine if it hasn't been declared yet

        if (new_seed != DEFAULT_RANDOM_SEED) { // If the new given seed is not the default seed then use it to seed the engine
                seed = new_seed;
                engine.seed(seed);
        } else if (seed == DEFAULT_RANDOM_SEED) { // If the current seed is the terrible default seed then seed the engine with the current time
                seed = time(NULL);
                engine.seed(seed);
        }

        if (minimum == 0) { // If the minimum is the lowest possible
                if (maximum == 0) { // If the maximum is equal to the minimum
                        return engine(); // Return any number
                }
                return engine() % maximum; // Return a number below the maximum value
        } else { // If the minimum exists
                if (maximum < minimum) { // If the maximum is less then the minimum, that signals that we should return the seed
                        return seed; // Return the static seed
                }
                return (engine() % (maximum - minimum)) + minimum; // Return a number bounded by the given minimum and maximum
        }
}
/*
* random() - Return a random number between 0 and the given number
* ! Note that if the given number is 0 then an unbounded random number will be returned
* @x: the maximum value to return
*/
unsigned int random(int x) {
        return random_internal(DEFAULT_RANDOM_SEED, 0, x);
}
/*
* random_range() - Return a random number between the two given values
* @x1: the minimum value to return
* @x2: the maximum value to return
*/
unsigned int random_range(unsigned int x1, unsigned int x2) {
        return random_internal(DEFAULT_RANDOM_SEED, x1, x2);
}
/*
* random_get_seed() - Return the current seed used in the random number engine
* ! Calling random_internal() with a larger minimum than the maximum signals it to return the seed
*/
unsigned int random_get_seed() {
        return random_internal(DEFAULT_RANDOM_SEED, 1, 0);
}
/*
* random_set_seed() - Set and return the seed used in the random number engine
* @new_seed: The new seed to generate random numbers with
*/
unsigned int random_set_seed(unsigned int new_seed) {
        return random_internal(new_seed, 1, 0);
}
/*
* random_reset_seed() - Set the seed of the random number engine to the current time
*/
unsigned int random_reset_seed() {
        return random_set_seed(time(NULL));
}
/*
* randomize() - Set the seed of the random number engine to a random number
*/
unsigned int randomize() {
        return random_set_seed(random_internal(DEFAULT_RANDOM_SEED, 0, 0));
}

/*
* sign() - Return -1, 0, or 1 based on the sign of the given number
* @x: the number to return the sign of
*/
template <typename T>
int sign(T x) {
        if (x > 0) {
                return 1; // Return 1 if the given number is greater than 0
        } else if (x < 0) {
                return -1; // Return -1 if the given number is less than 0
        }
        return 0; // Return 0 if the given number is equal to 0
}
template int sign<int>(int);
template int sign<double>(double);
/*
* sqr() - Return the square of the given number
* @x: the number to square
*/
template <typename T>
T sqr(T x) {
        return x*x; // Supposedly x*x is faster than pow(x, 2) but I doubt that it actually matters
}
template int sqr<int>(int);
template double sqr<double>(double);
/*
* logn() - Return the logarithm of the given number for any given base
* @n: the base of the logarithm
* @x: the number to log
*/
double logn(double n, double x) {
        return log(x)/log(n);
}
/*
* degtorad() - Convert the given number from degrees to radians
* @a: the number to convert
*/
double degtorad(double a) {
	return a*PI/180.0;
}
/*
* radtodeg() - Convert the given number from radians to degrees
* @a: the number to convert
*/
double radtodeg(double a) {
	return a*180.0/PI;
}
/*
* opposite_angle() - Return the angle (in degrees) which is opposite to the given one on the unit circle
* @a: the angle to find the opposite of
*/
double opposite_angle(double a) {
        a = absolute_angle(a); // Make sure that the angle is between 0.0 and 360.0
        if (a < 180.0) { // If the angle is on the top side of the unit circle then return the angle 180 degrees in front of it
                return a+180.0;
        }
        return a-180.0; // Otherwise, return the angle 180 degrees behind it
}
/*
* absolute_angle() - Return the angle absolute to the unit circle
* ! The primary use case is to correctly determine negative angles
* @a: the angle to find
*/
double absolute_angle(double a) {
        a = fmod(a, 360.0); // Make sure that the angle is between -360.0 and 360.0
        if (a < 0) {
                return a+360.0; // Return the equivalent angle if the given one is negative
        }
        return a; // Return the given angle
}

/*
* direction_of() - Return the direction in degrees from (x1, y1) to (x2, y2)
* @x1: the x-coordinate of the first point
* @y1: the y-coordinate of the first point
* @x2: the x-coordinate of the second point
* @y2: the y-coordinate of the second point
*/
double direction_of(double x1, double y1, double x2, double y2) {
	double dx = x2-x1;
	double dy = y2-y1;

	if (dx == 0.0) { // If the coordinates form a vertical line
		if (dy <= 0.0) {
			return 0.0; // Return 0.0 if the vector is pointing upwards
		} else {
			return 180.0; // Return 180.0 if the vector is pointing downwards
		}
	}

	return 90.0 + radtodeg(atan2(dy, dx)); // Otherwise calculate and return the angle of the vector
}
/*
* dist_sqr() - Return the square of the distance from (x1, y1) to (x2, y2) in order to avoid costly square roots
* @x1: the x-coordinate of the first point
* @y1: the y-coordinate of the first point
* @x2: the x-coordinate of the second point
* @y2: the y-coordinate of the second point
*/
double dist_sqr(double x1, double y1, double x2, double y2) {
        return sqr(x1-x2) + sqr(y1-y2);
}
/*
* distance() - Return the distance from (x1, y1) to (x2, y2)
* @x1: the x-coordinate of the first point
* @y1: the y-coordinate of the first point
* @x2: the x-coordinate of the second point
* @y2: the y-coordinate of the second point
*/
double distance(double x1, double y1, double x2, double y2) {
	return sqrt(sqr(x1-x2) + sqr(y1-y2));
}
/*
* coord_approach() - Return a pair of coordinates which is closer to (x2, y2) from (x1, y1) by a certain amount
* @x1: the x-coordinate of the original point
* @y1: the y-coordinate of the original point
* @x2: the x-coordinate of the desired destination
* @y2: the y-coordinate of the desired destination
* @speed: the factor by which to move towards the destination
*/
std::pair<int,int> coord_approach(int x1, int y1, int x2, int y2, int speed) {
	float d = distance(x1, y1, x2, y2);
	if (d <= speed) { // If the distance between the points is less than the speed, simply return the destination point
		return std::make_pair(x2, y2);
	}
 	float ratio = speed/d;

	int x3 = x1 + (x2-x1)*ratio;
	int y3 = y1 + (y2-y1)*ratio;

	return std::make_pair(x3, y3); // Return a point somewhere in between the given points based on the given speed
}

/*
* dot_product() - Return the dot product of two vectors given as individual elements
* @x1: the x-element of the first vector
* @y1: the y-element of the first vector
* @x2: the x-element of the second vector
* @y2: the y-element of the second vector
*/
template <typename T>
T dot_product(T x1, T y1, T x2, T y2) {
        return x1*x2 + y1*y2;
}
template int dot_product<int>(int, int, int, int);
template double dot_product<double>(double, double, double, double);
/*
* dot_product() - Return the dot product of two vectors given as pairs of elements
* @p1: the first vector
* @p2: the second vector
*/
template <typename T>
T dot_product(const std::pair<T,T>& p1, const std::pair<T,T>& p2) {
        return dot_product(p1.first, p1.second, p2.first, p2.second);
}
template int dot_product<int>(const std::pair<int,int>&, const std::pair<int,int>&);
template double dot_product<double>(const std::pair<double,double>&, const std::pair<double,double>&);

/*
* is_between() - Return whether the given number is between or equal to the given bounds
* @x: the number to check
* @a: one of the bounds
* @b: the other bound
*/
template <typename T>
bool is_between(T x, T a, T b) {
        if (a < b) { // Handle the case where the minimum is less than the maximum
                if ((x >= a)&&(x <= b)) {
                        return true; // Return true if x is in between the given bounds
                }
                return false;
        } else {
                return is_between(x, b, a);
        }
}
template bool is_between<int>(int, int, int);
template bool is_between<double>(double, double, double);
/*
* is_angle_between() - Return whether the given angle (in degrees) is between or equal to the given bounds
* @x: the angle to check
* @a: one of the bounds
* @b: the other bound
*/
template <typename T>
bool is_angle_between(T x, T a, T b) {
        x = absolute_angle(x); // Make sure the angle is between 0.0 and 360.0 degrees
        if (a < b) { // If the bounds are normal
                return is_between(x, a, b);
        } else { // If the bounds are at the top of the unit circle e.g. from 315 to 45
                return is_between(x, a, (T)360) || is_between(x, (T)0, b);
        }
}
template bool is_angle_between<int>(int, int, int);
template bool is_angle_between<double>(double, double, double);
/*
* fit_bounds() - Return a value in between the given bounds, either x or one of the bounds
* @x: the number to check whether it is between the bounds
* @a: one of the bounds
* @b: the other bound
*/
template <typename T>
T fit_bounds(T x, T a, T b) {
        if (a < b) { // Handle the case where the minimum is less than the maximum
                if (x < a) { // If x is less than the minimum then return the minimum
                        return a;
                }
                if (x > b) { // If x is greater than the maximum then return the maximum
                        return b;
                }
                return x; // Otherwise, return x
        } else {
                return fit_bounds(x, b, a);
        }
}
template int fit_bounds<int>(int, int, int);
template double fit_bounds<double>(double, double, double);

#endif // _BEE_UTIL_REAL_H
