/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_VIEWDATA_H
#define BEE_RENDER_VIEWDATA_H 1

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	// Forward declaration
	class Instance;

	struct ViewData {
		bool is_visible;

		SDL_Rect view;
		SDL_Rect port;

		Instance* following;

		int horizontal_border, vertical_border;
		int horizontal_speed, vertical_speed;

		ViewData();
		ViewData(bool, SDL_Rect, SDL_Rect, Instance*, int, int, int, int);
	};
}

#endif // BEE_RENDER_VIEWDATA_H
