/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_WINDOW_H
#define BEE_CORE_WINDOW_H 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	std::string get_window_title();
	SDL_Cursor* get_cursor();
	int get_window_x();
	int get_window_y();
	int get_width();
	int get_height();

	int set_window_title(const std::string&);
	int set_cursor(SDL_SystemCursor);
	int set_show_cursor(bool);
	int set_window_position(int, int);
	int set_window_x(int);
	int set_window_y(int);
	int set_window_center();
	int set_window_size(int, int);
	int set_width(int);
	int set_height(int);
}

#endif // BEE_CORE_WINDOW_H
