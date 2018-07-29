/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_WINDOW_H
#define BEE_CORE_WINDOW_H 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	std::string get_window_title();
	SDL_Rect get_window();
	std::pair<int,int> get_window_pos();
	std::pair<int,int> get_window_size();

	void set_window_title(const std::string&);
	void set_window_position(int, int);
	void set_window_center();
	void set_window_size(int, int);
}

#endif // BEE_CORE_WINDOW_H
