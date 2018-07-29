/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_DISPLAY_H
#define BEE_CORE_DISPLAY_H 1

#include <utility>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	SDL_DisplayMode get_display();
	std::pair<int,int> get_display_size();
	int set_display(int, int, int);
	int set_display_size(int, int);
	int set_display_refresh_rate(int);
}

#endif // BEE_CORE_DISPLAY_H
