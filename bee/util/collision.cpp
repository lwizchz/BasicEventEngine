/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_COLLISION
#define BEE_UTIL_COLLISION 1

// Collision checking functions

#include "collision.hpp" // Include the function definitions

#include "real.hpp" // Include the required real number functions

namespace util {

/**
* @see https://wiki.libsdl.org/SDL_Rect for details
* @param a one of the rectangles
* @param b the other rectangle
* @returns whether the given rectangles overlap
*/
bool check_collision(const SDL_Rect& a, const SDL_Rect& b) {
	// Define the boundaries for the first rectangle
	int a_left = a.x;
	int a_right = a.x + a.w;
	int a_top = a.y;
	int a_bottom = a.y + a.h;

	// Define the boundaries for the second rectangle
	int b_left = b.x;
	int b_right = b.x + b.w;
	int b_top = b.y;
	int b_bottom = b.y + b.h;

	// Compare the boundaries of the two rectangles
	if (a_bottom < b_top) {
		return false; // Return false if the bottom of the first rectangle is above the second rectangle
	} else if (a_top > b_bottom) {
		return false; // Return false if the first rectangle is below the bottom of the second rectangle
	} else if (a_right < b_left) {
		return false; // Return false if the right edge of the first rectangle is left of the second rectangle
	} else if (a_left > b_right) {
		return false; // Return false if the left edge of the first rectangle is right of the second rectangle
	}

	return true; // Otherwise, return true
}

/**
* @param a the angle to operate on
* @returns the given angle after reflecting it over the y-axis
*/
double angle_hbounce(double a) {
	a = absolute_angle(a); // Ensure that the given angle is between 0.0 and 360.0
	if ((a > 0)&&(a < 180)) {
		a = (90 - a) + 90;
	} else if ((a > 180)&&(a < 360)) {
		a = (270 - a) + 270;
	} else {
		a += 180;
	}
	return absolute_angle(a); // Return a new angle, ensuring that it is between 0.0 and 360.0
}
/**
* @param a the angle to operate on
* @returns the given angle after reflecting it over the x-axis
*/
double angle_vbounce(double a) {
	a = absolute_angle(a); // Ensure that the given angle is between 0.0 and 360.0
	if ((a > 0)&&(a < 90)) {
		a = (90 - a) + 270;
	} else if ((a > 270)&&(a < 360)) {
		a = 90 - (a - 270);
	} else if ((a > 90)&&(a < 270)) {
		a = (180 - a) + 180;
	} else {
		a += 180;
	}
	return absolute_angle(a); // Return a new angle, ensuring that it is between 0.0 and 360.0
}

}

#endif // BEE_UTIL_COLLISION
