/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_RGBA
#define BEE_RENDER_RGBA 1

#include <ostream>

#include "rgba.hpp"

#include "../util/real.hpp"

namespace bee {
	RGBA::RGBA(int nr, int ng, int nb, int na) :
		r(nr),
		g(ng),
		b(nb),
		a(na)
	{}
	RGBA::RGBA() :
		RGBA(0, 0, 0, 0)
	{}

	std::array<float,3> RGBA::get_hsv() const {
		float h = 0.f;
		float s = 0.f;
		float v = 0.f;

		const float red = r / 255.f;
		const float green = g / 255.f;
		const float blue = b / 255.f;

		const float M = std::max(red, std::max(green, blue));
		const float m = std::min(red, std::min(green, blue));
		const float chroma = M - m;

		float h_prime = 0.f;
		if (chroma != 0.f) {
			if (M == red) {
				h_prime = qmod((green-blue) / chroma, 6);
			} else if (M == green) {
				h_prime = (blue-red) / chroma + 2.f;
			} else if (M == blue) {
				h_prime = (red-green) / chroma + 4.f;
			}
		}
		h = h_prime * 60.f;
		if (h < 0) {
			h += 360.f;
		}

		v = M;

		if (M == 0.f) {
			s = 0.f;
		} else {
			s = chroma / M;
		}

		return {h, s, v};
	}
	float RGBA::get_hue() const {
		return get_hsv()[0];
	}
	float RGBA::get_saturation() const {
		return get_hsv()[1];
	}
	float RGBA::get_value() const {
		return get_hsv()[2];
	}

	int RGBA::set_hsv(const std::array<float,3>& hsv) {
		const float& h = hsv[0];
		const float& s = hsv[1];
		const float& v = hsv[2];

		const float chroma = v * s;
		const float h_prime = h / 60.f;
		const float x_color = chroma * (1.f - abs(qmod(h_prime, 2) - 1.f));

		float component_1 = 0.f;
		float component_2 = 0.f;
		float component_3 = 0.f;
		switch (static_cast<int>(h_prime)) {
			case 0: {
				component_1 = chroma;
				component_2 = x_color;
				break;
			}
			case 1: {
				component_1 = x_color;
				component_2 = chroma;
				break;
			}
			case 2: {
				component_2 = chroma;
				component_3 = x_color;
				break;
			}
			case 3: {
				component_2 = x_color;
				component_3 = chroma;
				break;
			}
			case 4: {
				component_1 = x_color;
				component_3 = chroma;
				break;
			}
			case 5: {
				component_1 = chroma;
				component_3 = x_color;
				break;
			}
			default: {
				break;
			}
		}

		const float m = v - chroma;
		r = static_cast<int>(round(255.f * (component_1 + m)));
		g = static_cast<int>(round(255.f * (component_2 + m)));
		b = static_cast<int>(round(255.f * (component_3 + m)));

		return 0;
	}
	int RGBA::set_hue(float h) {
		std::array<float,3> hsv = get_hsv();
		hsv[0] = h;
		return set_hsv(hsv);
	}
	int RGBA::set_saturation(float s) {
		std::array<float,3> hsv = get_hsv();
		hsv[1] = s;
		return set_hsv(hsv);
	}
	int RGBA::set_value(float v) {
		std::array<float,3> hsv = get_hsv();
		hsv[2] = v;
		return set_hsv(hsv);
	}
	int RGBA::add_hue(float h) {
		std::array<float,3> hsv = get_hsv();
		hsv[0] += h;
		return set_hsv(hsv);
	}
	int RGBA::add_saturation(float s) {
		std::array<float,3> hsv = get_hsv();
		hsv[1] += s;
		return set_hsv(hsv);
	}
	int RGBA::add_value(float v) {
		std::array<float,3> hsv = get_hsv();
		hsv[2] += v;
		return set_hsv(hsv);
	}

	bool RGBA::operator==(const RGBA& other) const {
		return (
			(this->r == other.r)
			&&(this->g == other.g)
			&&(this->b == other.b)
			&&(this->a == other.a)
		);
	}

	std::ostream& operator<<(std::ostream& os, const RGBA& rgba) {
		os << "{" <<
			static_cast<int>(rgba.r) << ", " <<
			static_cast<int>(rgba.g) << ", " <<
			static_cast<int>(rgba.b) << ", " <<
			static_cast<int>(rgba.a) <<
		"}";
		return os;
	}
}

#endif // BEE_RENDER_RGBA
