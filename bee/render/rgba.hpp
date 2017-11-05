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

		RGBA(int, int, int, int);
		RGBA();

		RGBA get_inverse();

		std::array<float,3> get_hsv() const;
		float get_hue() const;
		float get_saturation() const;
		float get_value() const;

		int set_hsv(const std::array<float,3>&);
		int set_hue(float);
		int set_saturation(float);
		int set_value(float);
		int add_hue(float);
		int add_saturation(float);
		int add_value(float);

		bool operator==(const RGBA&) const;

		friend std::ostream& operator<<(std::ostream&, const RGBA&);
	};
}

#endif // BEE_RENDER_RGBA_H
