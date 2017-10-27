/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_VIEWDATA
#define BEE_RENDER_VIEWDATA 1

#include "viewdata.hpp"

#include "../defines.hpp"

namespace bee {
	ViewData::ViewData() :
		ViewData(
			true,
			{0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT},
			{0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT},
			nullptr,
			0, 0,
			0, 0
		)
	{}
	ViewData::ViewData(bool _is_visible, SDL_Rect _view, SDL_Rect _port, Instance* _following, int _horizontal_border, int _vertical_border, int _horizontal_speed, int _vertical_speed) :
		is_visible(_is_visible),

		view(_view),
		port(_port),

		following(_following),

		horizontal_border(_horizontal_border),
		vertical_border(_vertical_border),
		horizontal_speed(_horizontal_speed),
		vertical_speed(_vertical_speed)
	{}
}

#endif // BEE_RENDER_VIEWDATA
