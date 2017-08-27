/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_RGBA_H
#define BEE_RENDER_RGBA_H 1

#include <array>

#include <SDL2/SDL.h>

namespace bee {
	struct RGBA {
		Uint8 r, g, b, a;

		RGBA();
		RGBA(int, int, int, int);

		std::array<int,3> get_hsv() const;
		int get_hue() const;
		int get_saturation() const;
		int get_value() const;

		int set_hsv(const std::array<int,3>&);
		int set_hue(int);
		int set_saturation(int);
		int set_value(int);
		int add_hue(int);
		int add_saturation(int);
		int add_value(int);
	};
}

#endif // BEE_RENDER_RGBA_H
