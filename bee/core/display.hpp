/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_DISPLAY_H
#define BEE_CORE_DISPLAY_H 1

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	SDL_DisplayMode get_display();
	Uint32 get_display_format();
	int get_display_width();
	int get_display_height();
	int get_display_refresh_rate();
	int set_display(int, int, int);
	int set_display_size(int, int);
	int set_display_refresh_rate(int);
}

#endif // BEE_CORE_DISPLAY_H
