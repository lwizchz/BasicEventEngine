/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_COLLISION_H
#define BEE_UTIL_COLLISION_H 1

#include <SDL2/SDL.h> // Include the required library headers

namespace util {
	bool check_collision(const SDL_Rect&, const SDL_Rect&);

	double angle_hbounce(double);
	double angle_vbounce(double);
}

#endif // BEE_UTIL_COLLISION_H
