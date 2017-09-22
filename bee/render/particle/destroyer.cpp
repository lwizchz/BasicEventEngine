/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_DESTROYER
#define BEE_RENDER_PARTICLE_DESTROYER 1

#include "destroyer.hpp"

#include "../../util/collision.hpp"

#include "../../core/instance.hpp"

#include "../drawing.hpp"
#include "particle.hpp"
#include "particledata.hpp"

namespace bee {
	ParticleDestroyer::ParticleDestroyer() :
		following(nullptr),

		x(0.0),
		y(0.0),
		w(1),
		h(1),

		shape(E_PS_SHAPE::RECTANGLE)
	{}
	ParticleDestroyer::ParticleDestroyer(double new_x, double new_y, unsigned int new_w, unsigned int new_h) :
		ParticleDestroyer()
	{
		x = new_x;
		y = new_y;
		w = (new_w < 1) ? 1 : new_w;
		h = (new_h < 1) ? 1 : new_h;
	}

	int ParticleDestroyer::set_following(Instance* inst) {
		following = inst;
		return 0;
	}

	double ParticleDestroyer::get_following_x(double default_x) {
		if (following != nullptr) {
			return following->get_x();
		}
		return default_x;
	}
	double ParticleDestroyer::get_following_y(double default_y) {
		if (following != nullptr) {
			return following->get_y();
		}
		return default_y;
	}

	bool ParticleDestroyer::handle(ParticleData* pd, double system_x, double system_y) {
		double dx = get_following_x(system_x);
		double dy = get_following_y(system_y);

		SDL_Rect a = pd->get_rect();
		SDL_Rect b = {static_cast<int>(dx+x), static_cast<int>(dy+y), static_cast<int>(w), static_cast<int>(h)};
		if (check_collision(a, b)) {
			return true;
		}

		return false;
	}

	int ParticleDestroyer::draw_debug(double system_x, double system_y, E_RGB color) {
		double dx = get_following_x(system_x);
		double dy = get_following_y(system_y);

		return draw_rectangle(static_cast<int>(dx+x), static_cast<int>(dy+y), w, h, 1, get_enum_color(color));
	}
}

#endif // BEE_RENDER_PARTICLE_DESTROYER
