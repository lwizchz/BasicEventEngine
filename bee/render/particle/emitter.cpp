/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_EMITTER
#define BEE_RENDER_PARTICLE_EMITTER 1

#include "emitter.hpp"

#include "../../util/real.hpp"

#include "../../core/instance.hpp"

#include "../drawing.hpp"
#include "system.hpp"

namespace bee {
	ParticleEmitter::ParticleEmitter() :
		following(nullptr),

		x(0.0),
		y(0.0),
		w(1),
		h(1),

		shape(E_PS_SHAPE::RECTANGLE),
		distribution(E_PS_DISTR::LINEAR),

		particle_type(nullptr),

		number(1),
		number_count(0)
	{}
	ParticleEmitter::ParticleEmitter(double new_x, double new_y, unsigned int new_w, unsigned int new_h, Particle* ptype) :
		ParticleEmitter()
	{
		x = new_x;
		y = new_y;
		w = (new_w < 1) ? 1 : new_w;
		h = (new_h < 1) ? 1 : new_h;

		particle_type = ptype;
	}

	int ParticleEmitter::set_following(Instance* inst) {
		following = inst;
		return 0;
	}
	int ParticleEmitter::set_number(int new_number) {
		number = new_number;
		return 0;
	}

	double ParticleEmitter::get_following_x(double default_x) {
		if (following != nullptr) {
			return following->get_x();
		}
		return default_x;
	}
	double ParticleEmitter::get_following_y(double default_y) {
		if (following != nullptr) {
			return following->get_y();
		}
		return default_y;
	}

	int ParticleEmitter::emit(ParticleSystem* sys, double xoffset, double yoffset) {
		return sys->add_particle(particle_type, xoffset + x + random(w), yoffset + y + random(h));
	}
	int ParticleEmitter::handle(ParticleSystem* sys, double system_x, double system_y) {
		int ex = get_following_x(system_x);
		int ey = get_following_y(system_y);

		if (number >= 0) {
			for (int i=0; i<number; i++) {
				emit(sys, ex, ey);
			}
		} else {
			if (number_count++ >= -number) {
				emit(sys, ex, ey);
				number_count = 0;
			}
		}

		return 0;
	}

	int ParticleEmitter::draw_debug(double system_x, double system_y, E_RGB color) {
		double ex = get_following_x(system_x);
		double ey = get_following_y(system_y);

		return draw_rectangle(ex+x, ey+y, w, h, 1, get_enum_color(color));
	}
}

#endif // BEE_RENDER_PARTICLE_EMITTER
