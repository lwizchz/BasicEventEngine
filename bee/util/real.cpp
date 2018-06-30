/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_REAL
#define BEE_UTIL_REAL 1

// Real number functions

#include <random>
#include <time.h>

#include "real.hpp" // Include the function definitions

#include <glm/gtc/type_ptr.hpp>

namespace util {

/**
* @param x the number to return the sign of
*
* @retval -1 the number is negative
* @retval 0 the number is zero
* @retval 1 the number is positive
*/
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
template int sign<long>(long);
template int sign<float>(float);
template int sign<double>(double);
#ifdef _WIN32
	template int sign<time_t>(time_t); // On Windows time_t is __int64, but on Linux time_t is long
#endif
/**
* @param x the number to square
*
* @returns the square of the given number
*/
template <typename T>
T sqr(T x) {
	return x*x; // Supposedly x*x is faster than pow(x, 2) but I doubt that it actually matters
}
template int sqr<int>(int);
template long sqr<long>(long);
template float sqr<float>(float);
template double sqr<double>(double);
/**
* @param n the base of the logarithm
* @param x the number to log
*
* @returns the logarithm of the given number for any given base
*/
double logn(double n, double x) {
	return log(x)/log(n);
}
/**
* Convert the given number from degrees to radians.
* @param a the number to convert
*
* @returns the equivalent angle in radians
*/
double degtorad(double a) {
	return a*PI/180.0;
}
/**
* Convert the given number from radians to degrees.
* @param a the number to convert
*
* @returns the equivalent angle in degrees
*/
double radtodeg(double a) {
	return a*180.0/PI;
}
/**
* @param a the angle to find the opposite of
*
* @returns the angle (in degrees) which is opposite to the given one on the unit circle
*/
double opposite_angle(double a) {
	a = absolute_angle(a); // Ensure that the angle is between 0.0 and 360.0
	if (a < 180.0) { // If the angle is on the top side of the unit circle then return the angle 180 degrees in front of it
		return a+180.0;
	}
	return a-180.0; // Otherwise, return the angle 180 degrees behind it
}
/**
* @note The primary use case of this function is to correctly determine negative angles.
* @param a the angle to find
*
* @returns the angle absolute to the unit circle
*/
double absolute_angle(double a) {
	a = fmod(a, 360.0); // Ensure that the angle is between -360.0 and 360.0
	if (a < 0) {
		return a+360.0; // Return the equivalent angle if the given one is negative
	}
	return a;
}

/**
* @param v1 the position vector of the first point
* @param v2 the position vector of the second point
*
* @returns the direction as a unit vector from v1 to v2
*/
btVector3 direction_of(btVector3 v1, btVector3 v2) {
	return btVector3(
		v2.x()-v1.x(),
		v2.y()-v1.y(),
		v2.z()-v1.z()
	).normalized();
}
/**
* @param (x1, y1) the coordinates of the first point
* @param (x2, y2) the coordinates of the second point
*
* @returns the direction in degrees from (x1, y1) to (x2, y2)
*/
double direction_of(double x1, double y1, double x2, double y2) {
	return absolute_angle(radtodeg(atan2(y2-y1, x2-x1)));
}
/**
* @note If the function is called without the z-coordinates, then let them equal 0.0
* @param (x1, y1) the coordinates of the first point
* @param (x2, y2) the coordinates of the second point
*
* @returns the square of the distance from (x1, y1, 0.0) to (x2, y2, 0.0) in order to avoid a costly square root
*/
double dist_sqr(double x1, double y1, double x2, double y2) {
	btVector3 v1 (static_cast<float>(x1), static_cast<float>(y1), 0.0f);
	btVector3 v2 (static_cast<float>(x2), static_cast<float>(y2), 0.0f);
	return v1.distance2(v2);
}
/**
* @note If the function is called without the z-coordinates, then let them equal 0.0
* @param (x1, y1, z1) the coordinates of the first point
* @param (x2, y2, z2) the coordinates of the second point
*
* @returns the distance from (x1, y1) to (x2, y2)
*/
double distance(double x1, double y1, double x2, double y2) {
	return sqrt(dist_sqr(
		static_cast<float>(x1), static_cast<float>(y1),
		static_cast<float>(x2), static_cast<float>(y2)
	));
}
/**
* @param (x1, y1) the coordinates of the first point
* @param (x2, y2) the coordinates of the second point
* @param speed the factor by which to move towards the destination
* @param dt the delta time
*
* @returns a pair of coordinates which is closer to (x2, y2) from (x1, y1) by a certain amount
*/
std::pair<double,double> coord_approach(double x1, double y1, double x2, double y2, double speed, double dt) {
	double d = distance(x1, y1, x2, y2);
	if (d <= speed) { // If the distance between the points is less than the speed, simply return the destination point
		return std::make_pair(x2, y2);
	}
 	double ratio = speed/d;

	double x3 = x1 + (x2-x1)*ratio*dt;
	double y3 = y1 + (y2-y1)*ratio*dt;

	return std::make_pair(x3, y3); // Return a point somewhere in between the given points based on the given speed
}

/**
* @param v the vector to convert
*
* @returns the equivalent vector as a glm::vec3
*/
glm::vec3 bt_to_glm_v3(const btVector3& v) {
	return glm::vec3(v.x(), v.y(), v.z());
}
/**
* @param v the vector to convert
*
* @returns the equivalent vector as a btVector3
*/
btVector3 glm_to_bt_v3(const glm::vec3& v) {
	return btVector3(v.x, v.y, v.z);
}
/**
* @param v the vector to convert
*
* @returns the equivalent vector as a glm::vec3
*/
glm::vec3 ai_to_glm_v3(const aiVector3D& v) {
	return glm::vec3(v.x, v.y, v.z);
}
/**
* @param m the matrix to convert
*
* @returns the equivalent matrix as a glm::mat4
*/
glm::mat4 ai_to_glm_m4(const aiMatrix4x4& m) {
	float _m[16] = {
		// Row major
		/*m.a1, m.a2, m.a3, m.a4,
		m.b1, m.b2, m.b3, m.b4,
		m.c1, m.c2, m.c3, m.c4,
		m.d1, m.d2, m.d3, m.d4*/

		// Column major
		m.a1, m.b1, m.c1, m.d1,
		m.a2, m.b2, m.c2, m.d2,
		m.a3, m.b3, m.c3, m.d3,
		m.a4, m.b4, m.c4, m.d4
	};
	return glm::make_mat4(_m);
}
glm::mat4 ai_to_glm_m4(const aiQuaternion&q) {
	return ai_to_glm_m4(aiMatrix4x4(q.GetMatrix()));
}

/**
* Linearly interpolate between the given values.
* @param a the first value
* @param b the second value
* @param t the percentage from 0.0 to 1.0
*/
template <typename T>
T interp_linear(T a, T b, double t) {
	return a + (b - a) * t;
}
template int interp_linear(int, int, double);
template long interp_linear(long, long, double);
template float interp_linear(float, float, double);
template double interp_linear(double, double, double);

/**
* @param x the number to check
* @param a the lower bound
* @param b the upper bound
*
* @returns whether the given number is between or equal to the given bounds
*/
template <typename T>
bool is_between(T x, T a, T b) {
	if (a < b) { // Handle the case where the minimum is less than the maximum
		if ((x >= a)&&(x <= b)) {
			return true; // Return true if x is in between the given bounds
		}
	} else if ((a == b)&&(x == a)) {
		return true;
	}

	return false;
}
template bool is_between<int>(int, int, int);
template bool is_between<long>(long, long, long);
template bool is_between<float>(float, float, float);
template bool is_between<double>(double, double, double);
/**
* @param x the angle to check
* @param a the lower bound
* @param b the upper bound
*
* @returns whether the given angle (in degrees) is between or equal to the given bounds
*/
template <typename T>
bool is_angle_between(T x, T a, T b) {
	// Ensure the angles are between 0.0 and 360.0 degrees
	x = static_cast<T>(absolute_angle(x));
	a = static_cast<T>(absolute_angle(a));
	b = static_cast<T>(absolute_angle(b));

	if (a < b) { // If the bounds are normal
		return is_between(x, a, b);
	} else { // If the bounds are at the top of the unit circle e.g. from 315 to 45
		return is_between(x, a, static_cast<T>(360)) || is_between(x, static_cast<T>(0), b);
	}
}
template bool is_angle_between<int>(int, int, int);
template bool is_angle_between<long>(long, long, long);
template bool is_angle_between<float>(float, float, float);
template bool is_angle_between<double>(double, double, double);
/**
* @param x the number to check
* @param a the lower bound
* @param b the upper bound
*
* @returns a value in between the given bounds, either x or one of the bounds
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
	} else if (a == b) {
		return a;
	} else {
		if ((x >= a)||(x <= b)) {
			return x;
		}

		if (a-x < x-b) {
			return a;
		}

		return b;
	}
}
template int fit_bounds<int>(int, int, int);
template long fit_bounds<long>(long, long, long);
template float fit_bounds<float>(float, float, float);
template double fit_bounds<double>(double, double, double);
/**
* @note This can used to avoid unnecessary floating point divisions. The result is effectively:
* @code x - truncated(x/m) * m @endcode
* @param x the quotient numerator
* @param m the quotient denominator
*
* @returns the result of a "quick" modulo
*/
template <typename T>
T qmod(T x, unsigned int m) {
	if ((m == 0)||(m == 1)) {
		return m;
	}

	if (x < 0) { // Handle negative modulo
		while (x < 0) {
			x += m;
		}
		return x;
	}

	while (x >= 0) {
		x -= m;
	}
	return x+m;
}
template int qmod<int>(int, unsigned int);
template long qmod<long>(long, unsigned int);
template float qmod<float>(float, unsigned int);
template double qmod<double>(double, unsigned int);

namespace random { namespace internal {
	const unsigned int default_seed = 1;
	unsigned int seed = default_seed;
	std::mt19937 engine;
}}
/**
* @param min the minimum value to return
* @param max the maximum value to return
*
* @returns a random number between the two given values
*/
unsigned int random::get_range(unsigned int min, unsigned int max) {
	if (min == 0) { // If the minimum is the lowest possible
		if (max == 0) { // If the maximum is equal to the minimum
			return internal::engine(); // Return any number
		}
		return internal::engine() % max; // Return a number below the maximum value
	} else { // If the minimum exists
		return (internal::engine() % (max - min)) + min; // Return a number bounded by the given minimum and maximum
	}
}
/**
* @note If the given number is 0 then an unbounded random number will be returned.
* @param max the maximum value to return
*
* @returns a random number between 0 and the given number
*/
unsigned int random::get(int max) {
	return get_range(0, max);
}
/**
* @note Calling random_internal() with a larger minimum than the maximum signals it to return the seed.
* @returns the current seed used in the random number engine
*/
unsigned int random::get_seed() {
	return internal::seed;
}
/**
* Set the seed used in the random number engine.
* @param new_seed the new seed to generate random numbers with
*
* @returns the new seed
*/
unsigned int random::set_seed(unsigned int new_seed) {
	if (new_seed != internal::default_seed) { // If the new given seed is not the default seed then use it to seed the engine
		internal::seed = new_seed;
		internal::engine.seed(internal::seed);
	} else if (internal::seed == internal::default_seed) { // If the current seed is the terrible default seed then seed the engine with the current time
		internal::seed = static_cast<unsigned int>(time(nullptr));
		internal::engine.seed(internal::seed);
	}

	return internal::seed;
}
/**
* Set the seed of the random number engine to the current time.
*
* @returns the new seed
*/
unsigned int random::reset_seed() {
	return set_seed(static_cast<unsigned int>(time(nullptr)));
}
/**
* Set the seed of the random number engine to a random number.
*
* @returns the new seed
*/
unsigned int random::randomize() {
	return set_seed(get(0));
}

namespace checksum { namespace internal {
	std::vector<unsigned int> crc_table;
}}
/**
* Reflect the CRC table value to conform to the CRC standard.
* @param reflect the value to be reflected
* @param bits the number of bits to reflect
*
* @returns the reflected value
*/
unsigned int checksum::internal::reflect(unsigned int reflect, const char bits) {
	unsigned int value = 0;

	for (int i=0; i<(bits+1); ++i) { // Swap bits
		if (reflect & 1) {
			value |= 1 << ((bits-i) % 32);
		}
		reflect >>= 1;
	}

	return value;
}
/**
* @param index the index of the value to return
*
* @returns a value from the CRC lookup table
*/
unsigned int checksum::internal::table(size_t index) {
	if (crc_table.empty()) {
		unsigned int polynomial = 0x04C11DB7; // Use the official polynomial used by most implementations

		crc_table.resize(256);
		for (unsigned int i=0; i<256; ++i) {
			crc_table[i] = internal::reflect(i, 8) << 24;

			for (unsigned int j=0; j<8; ++j) {
				crc_table[i] =
					(crc_table[i] << 1)
					^ (
						(crc_table[i] & (1 << 31)) ? polynomial : 0
					);
			}

			crc_table[i] = internal::reflect(crc_table[i], 32);
		}
	}

	return crc_table[index];
}
/**
* @param data the data vector to generate a checksum for
*
* @returns the CRC32 checksum for the given data
*/
unsigned int checksum::get(const std::vector<unsigned char>& data) {
	unsigned int crc = 0xffffffff; // Initialize the checksum

	for (auto& d : data) {
		crc = (crc >> 8) ^ internal::table((crc & 0xff) ^ d);
	}

	return (crc ^ 0xffffffff); // Finalize and return the checksum
}
/**
* @param data the data to check
* @param crc the checksum to verify against
*
* @returns whether the data matches the checksum
*/
bool checksum::verify(const std::vector<unsigned char>& data, unsigned int crc) {
	return (get(data) == crc);
}

}

#endif // BEE_UTIL_REAL
