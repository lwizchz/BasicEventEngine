/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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
	ParticleDestroyer::ParticleDestroyer(double x, double y, unsigned int w, unsigned int h) :
		region(nullptr, x, y, w, h),

		shape(E_PS_SHAPE::RECTANGLE)
	{}

	int ParticleDestroyer::set_following(Instance* inst) {
		region.following = inst;
		return 0;
	}

	double ParticleDestroyer::get_following_x(double default_x) {
		if (region.following != nullptr) {
			return region.following->get_x();
		}
		return default_x;
	}
	double ParticleDestroyer::get_following_y(double default_y) {
		if (region.following != nullptr) {
			return region.following->get_y();
		}
		return default_y;
	}

	bool ParticleDestroyer::handle(ParticleData* pd, double system_x, double system_y) {
		double dx = get_following_x(system_x);
		double dy = get_following_y(system_y);

		SDL_Rect a = pd->get_rect();
		SDL_Rect b = {static_cast<int>(dx+region.x), static_cast<int>(dy+region.y), static_cast<int>(region.w), static_cast<int>(region.h)};
		if (util::check_collision(a, b)) {
			return true;
		}

		return false;
	}

	int ParticleDestroyer::draw_debug(double system_x, double system_y, E_RGB color) {
		double dx = get_following_x(system_x);
		double dy = get_following_y(system_y);

		return draw_rectangle(static_cast<int>(dx+region.x), static_cast<int>(dy+region.y), region.w, region.h, 1, RGBA(color));
	}
}

#endif // BEE_RENDER_PARTICLE_DESTROYER
