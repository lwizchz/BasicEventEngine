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
	ParticleChanger::ParticleChanger(double _x, double _y, unsigned int _w, unsigned int _h, Particle* _part_before, Particle* _part_after) :
		ParticleChanger()
	{
		x = _x;
		y = _y;
		w = (_w < 1) ? 1 : _w;
		h = (_h < 1) ? 1 : _h;

		particle_before = _part_before;
		particle_after = _part_after;
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

		return draw_rectangle(static_cast<int>(cx+x), static_cast<int>(cy+y), w, h, 1, get_enum_color(color));
	}
}

#endif // BEE_RENDER_PARTICLE_CHANGER
