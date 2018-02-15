/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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
	ParticleDeflector::ParticleDeflector(double x, double y, unsigned int w, unsigned int h) :
		region(nullptr, x, y, w, h),

		friction(1.0)
	{}

	int ParticleDeflector::set_following(Instance* inst) {
		region.following = inst;
		return 0;
	}
	int ParticleDeflector::set_friction(double _friction) {
		friction = _friction;
		return 0;
	}

	double ParticleDeflector::get_following_x(double default_x) {
		if (region.following != nullptr) {
			return region.following->get_x();
		}
		return default_x;
	}
	double ParticleDeflector::get_following_y(double default_y) {
		if (region.following != nullptr) {
			return region.following->get_y();
		}
		return default_y;
	}

	int ParticleDeflector::handle(ParticleData* pd, double old_px, double old_py, double system_x, double system_y) {
		double dx = get_following_x(system_x);
		double dy = get_following_y(system_y);

		SDL_Rect a = pd->get_rect();
		SDL_Rect b = {static_cast<int>(dx+region.x), static_cast<int>(dy+region.y), static_cast<int>(region.w), static_cast<int>(region.h)};
		if (util::check_collision(a, b)) {
			double dir = util::direction_of(old_px, old_py, pd->x, pd->y);
			pd->set_velocity(pd->velocity.first * friction * -1, dir);
		}

		return 0;
	}

	int ParticleDeflector::draw_debug(double system_x, double system_y, E_RGB color) {
		double dx = get_following_x(system_x);
		double dy = get_following_y(system_y);

		return draw_rectangle(static_cast<int>(dx+region.x), static_cast<int>(dy+region.y), region.w, region.h, 1, RGBA(color));
	}
}

#endif // BEE_RENDER_PARTICLE_DEFLECTOR
