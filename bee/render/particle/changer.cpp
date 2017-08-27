/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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
	ParticleChanger::ParticleChanger() :
		following(nullptr),

		particle_before(nullptr),
		particle_after(nullptr),

		x(0.0),
		y(0.0),
		w(1), h(1),

		shape(E_PS_SHAPE::RECTANGLE),

		change_type(E_PS_CHANGE::ALL)
	{}
	ParticleChanger::ParticleChanger(double new_x, double new_y, unsigned int new_w, unsigned int new_h, Particle* new_part_before, Particle* new_part_after) :
		ParticleChanger()
	{
		x = new_x;
		y = new_y;
		w = (new_w < 1) ? 1 : new_w;
		h = (new_h < 1) ? 1 : new_h;

		particle_before = new_part_before;
		particle_after = new_part_after;
	}

	int ParticleChanger::set_following(Instance* inst) {
		following = inst;
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
		if (following != nullptr) {
			return following->get_x();
		}
		return default_x;
	}
	double ParticleChanger::get_following_y(double default_y) {
		if (following != nullptr) {
			return following->get_y();
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
		SDL_Rect b = {static_cast<int>(cx+x), static_cast<int>(cy+y), static_cast<int>(w), static_cast<int>(h)};
		if (check_collision(a, b)) {
			if (pd->get_type() == get_part_before()) {
				pd->set_type(get_part_after());
			}
		}

		return 0;
	}

	int ParticleChanger::draw_debug(double system_x, double system_y, E_RGB color) {
		double cx = get_following_x(system_x);
		double cy = get_following_y(system_y);

		return draw_rectangle(cx+x, cy+y, w, h, 1, get_enum_color(color));
	}
}

#endif // BEE_RENDER_PARTICLE_CHANGER
