/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_CHANGER
#define BEE_RENDER_PARTICLE_CHANGER 1

#include "changer.hpp"

#include "../../util/collision.hpp"

#include "../../core/instance.hpp"

#include "../drawing.hpp"
#include "particledata.hpp"

namespace bee {
	ParticleChanger::ParticleChanger(double x, double y, unsigned int w, unsigned int h, Particle* _particle_before, Particle* _particle_after) :
		region(nullptr, x, y, w, h),

		particle_before(_particle_before),
		particle_after(_particle_after),

		shape(E_PS_SHAPE::RECTANGLE),

		change_type(E_PS_CHANGE::ALL)
	{}

	int ParticleChanger::set_following(Instance* inst) {
		region.following = inst;
		return 0;
	}
	int ParticleChanger::set_part_before(Particle* part) {
		particle_before = part;
		return 0;
	}
	int ParticleChanger::set_part_after(Particle* part) {
		particle_after = part;
		return 0;
	}

	double ParticleChanger::get_following_x(double default_x) {
		if (region.following != nullptr) {
			return region.following->get_x();
		}
		return default_x;
	}
	double ParticleChanger::get_following_y(double default_y) {
		if (region.following != nullptr) {
			return region.following->get_y();
		}
		return default_y;
	}
	Particle* ParticleChanger::get_part_before() {
		return particle_before;
	}
	Particle* ParticleChanger::get_part_after() {
		return particle_after;
	}

	int ParticleChanger::handle(ParticleData* pd, double system_x, double system_y) {
		double cx = get_following_x(system_x);
		double cy = get_following_y(system_y);

		SDL_Rect a = pd->get_rect();
		SDL_Rect b = {static_cast<int>(cx+region.x), static_cast<int>(cy+region.y), static_cast<int>(region.w), static_cast<int>(region.h)};
		if (util::check_collision(a, b)) {
			if (pd->get_type() == get_part_before()) {
				pd->set_type(get_part_after());
			}
		}

		return 0;
	}

	int ParticleChanger::draw_debug(double system_x, double system_y, E_RGB color) {
		double cx = get_following_x(system_x);
		double cy = get_following_y(system_y);

		return draw_rectangle(static_cast<int>(cx+region.x), static_cast<int>(cy+region.y), region.w, region.h, 1, RGBA(color));
	}
}

#endif // BEE_RENDER_PARTICLE_CHANGER
