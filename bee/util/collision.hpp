/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_COLLISION_H
#define BEE_UTIL_COLLISION_H 1

#include <SDL2/SDL.h> // Include the required library headers

// Define a struct for line data
class Line {
	public:
		double x1, y1, x2, y2;
};

bool compare_lines(const Line&, const Line&);

bool check_collision(const SDL_Rect&, const SDL_Rect&);
bool check_collision_circle(double, double, double, double, double, double);
bool check_collision_line(const Line&, const Line&);
bool check_collision_aligned_line(const Line&, const Line&);
bool check_collision(const SDL_Rect&, const Line&);

double angle_hbounce(double);
double angle_vbounce(double);

#endif // BEE_UTIL_COLLISION_H
