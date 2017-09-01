/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_RGBA
#define BEE_RENDER_RGBA 1

#include "rgba.hpp"

#include "../util/template/real.hpp"

namespace bee {
	RGBA::RGBA() :
		r(0),
		g(0),
		b(0),
		a(0)
	{}
	RGBA::RGBA(int nr, int ng, int nb, int na) :
		r(nr),
		g(ng),
		b(nb),
		a(na)
	{}

	std::array<int,3> RGBA::get_hsv() const {
		int h = 0;
		int s = 0;
		int v = 0;

		const int M = max<int>(r, g, b);
		const int m = min<int>(r, g, b);
		const int chroma = (M - m) * 100 / 255;

		int h_prime = 0;
		if (chroma != 0) {
			if (M == r) {
				h_prime = ((g-b) * 100 / 255 * 100 / chroma) % (6 * 100);
			} else if (M == g) {
				h_prime = (b-r) * 100 / 255 * 100 / chroma + 2 * 200;
			} else if (M == b) {
				h_prime = (r-g) * 100 / 255 * 100 / chroma + 4 * 100;
			}
		}
		h = h_prime * 60 / 100;

		v = M * 100 / 255;

		if (v == 0) {
			s = 0;
		} else {
			s = chroma * 100 / v;
		}

		return {h, s, v};
	}
	int RGBA::get_hue() const {
		return get_hsv()[0];
	}
	int RGBA::get_saturation() const {
		return get_hsv()[1];
	}
	int RGBA::get_value() const {
		return get_hsv()[2];
	}

	int RGBA::set_hsv(const std::array<int,3>& hsv) {
		int h = hsv[0];
		int s = hsv[1];
		int v = hsv[2];

		const int chroma = v * s / 100;
		const int h_prime = (h % 360) * 100 / 60;
		const int x_color = chroma * (100 - abs((h_prime % 200) - 100)) / 100;

		RGBA rgba_prime;
		switch (h_prime/100) {
			case 0: {
				rgba_prime = {chroma, x_color, 0, 255};
				break;
			}
			case 1: {
				rgba_prime = {x_color, chroma, 0, 255};
				break;
			}
			case 2: {
				rgba_prime = {0, chroma, x_color, 255};
				break;
			}
			case 3: {
				rgba_prime = {0, x_color, chroma, 255};
				break;
			}
			case 4: {
				rgba_prime = {x_color, 0, chroma, 255};
				break;
			}
			case 5: {
				rgba_prime = {chroma, 0, x_color, 255};
				break;
			}
			default: {
				rgba_prime = {0, 0, 0, 255};
				break;
			}
		}

		const int m = v - chroma;
		const int red = (rgba_prime.r + m) * 255 / 100;
		const int green = (rgba_prime.g + m) * 255 / 100;
		const int blue = (rgba_prime.b + m) * 255 / 100;

		r = fit_bounds(red, 0, 255);
		g = fit_bounds(green, 0, 255);
		b = fit_bounds(blue, 0, 255);

		return 0;
	}
	int RGBA::set_hue(int h) {
		std::array<int,3> hsv = get_hsv();
		hsv[0] = h;
		return set_hsv(hsv);
	}
	int RGBA::set_saturation(int s) {
		std::array<int,3> hsv = get_hsv();
		hsv[1] = s;
		return set_hsv(hsv);
	}
	int RGBA::set_value(int v) {
		std::array<int,3> hsv = get_hsv();
		hsv[2] = v;
		return set_hsv(hsv);
	}
	int RGBA::add_hue(int h) {
		std::array<int,3> hsv = get_hsv();
		hsv[0] += h;
		return set_hsv(hsv);
	}
	int RGBA::add_saturation(int s) {
		std::array<int,3> hsv = get_hsv();
		hsv[1] += s;
		return set_hsv(hsv);
	}
	int RGBA::add_value(int v) {
		std::array<int,3> hsv = get_hsv();
		hsv[2] += v;
		return set_hsv(hsv);
	}

	bool RGBA::operator==(const RGBA& other) {
		return (
			(this->r == other.r)
			&&(this->g == other.g)
			&&(this->b == other.b)
			&&(this->a == other.a)
		);
	}
}

#endif // BEE_RENDER_RGBA
