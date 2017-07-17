/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_DEFLECTOR
#define BEE_RENDER_PARTICLE_DEFLECTOR 1

#include "deflector.hpp"

#include "../../util/real.hpp"
#include "../../util/collision.hpp"

#include "../../core/instance.hpp"

#include "../drawing.hpp"
#include "particledata.hpp"

namespace bee {
	ParticleDeflector::ParticleDeflector() :
		following(nullptr),

		x(0.0),
		y(0.0),
		w(1),
		h(1),

		friction(1.0)
	{}
	ParticleDeflector::ParticleDeflector(double new_x, double new_y, unsigned int new_w, unsigned int new_h) :
		ParticleDeflector()
	{
		x = new_x;
		y = new_y;
		w = (new_w < 1) ? 1 : new_w;
		h = (new_h < 1) ? 1 : new_h;
	}

	int ParticleDeflector::set_following(Instance* inst) {
		following = inst;
		return 0;
	}
	int ParticleDeflector::set_friction(double new_friction) {
		friction = new_friction;
		return 0;
	}

	double ParticleDeflector::get_following_x(double default_x) {
		if (following != nullptr) {
			return following->get_x();
		}
		return default_x;
	}
	double ParticleDeflector::get_following_y(double default_y) {
		if (following != nullptr) {
			return following->get_y();
		}
		return default_y;
	}

	int ParticleDeflector::handle(ParticleData* pd, double old_px, double old_py, double system_x, double system_y) {
		int dx = get_following_x(system_x);
		int dy = get_following_y(system_y);

		SDL_Rect a = pd->get_rect();
		SDL_Rect b = {static_cast<int>(dx+x), static_cast<int>(dy+y), static_cast<int>(w), static_cast<int>(h)};
		if (check_collision(a, b)) {
			double dir = direction_of(old_px, old_py, pd->x, pd->y);
			pd->set_velocity(pd->velocity.first * friction * -1, dir);
		}

		return 0;
	}

	int ParticleDeflector::draw_debug(double system_x, double system_y, E_RGB color) {
		int dx = get_following_x(system_x);
		int dy = get_following_y(system_y);

		return draw_rectangle(dx+x, dy+y, w, h, false, color);
	}
}

#endif // BEE_RENDER_PARTICLE_DEFLECTOR
