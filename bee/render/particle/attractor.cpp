/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_ATTRACTOR
#define BEE_RENDER_PARTICLE_ATTRACTOR 1

#include "attractor.hpp"

#include "../../util/real.hpp"

#include "../../core/instance.hpp"

#include "../drawing.hpp"
#include "particledata.hpp"

namespace bee {
	ParticleAttractor::ParticleAttractor() :
		following(nullptr),

		x(0.0),
		y(0.0),
		w(1),
		h(1),

		force(50.0),
		max_distance(100.0),
		force_type(E_PS_FORCE::LINEAR)
	{}
	ParticleAttractor::ParticleAttractor(double new_x, double new_y, unsigned int new_w, unsigned int new_h) :
		ParticleAttractor()
	{
		x = new_x;
		y = new_y;
		w = (new_w < 1) ? 1 : new_w;
		h = (new_h < 1) ? 1 : new_h;
	}

	int ParticleAttractor::set_following(Instance* inst) {
		following = inst;
		return 0;
	}
	int ParticleAttractor::set_force(double new_force) {
		force = new_force;
		return 0;
	}
	int ParticleAttractor::set_max_distance(double new_max_distance) {
		max_distance = new_max_distance;
		return 0;
	}

	double ParticleAttractor::get_following_x(double default_x) {
		if (following != nullptr) {
			return following->get_x();
		}
		return default_x;
	}
	double ParticleAttractor::get_following_y(double default_y) {
		if (following != nullptr) {
			return following->get_y();
		}
		return default_y;
	}

	int ParticleAttractor::handle(ParticleData* pd, double system_x, double system_y, double delta) {
		int ax = get_following_x(system_x);
		int ay = get_following_y(system_y);

		double ds = dist_sqr(pd->x, pd->y, ax+x+w/2, ay+y+h/2);
		if (ds < sqr(max_distance)) {
			double f = 0.0;
			switch (force_type) {
				case E_PS_FORCE::CONSTANT: {
					f = force * pd->get_deviation_percent();
					break;
				}
				case E_PS_FORCE::LINEAR:
				default: {
					f = force * pd->get_deviation_percent() * (sqr(max_distance) - ds) / sqr(max_distance);
					break;
				}
				case E_PS_FORCE::QUADRATIC: {
					f = force * pd->get_deviation_percent() * sqr((sqr(max_distance) - ds) / sqr(max_distance));
					break;
				}
			}

			pd->set_position(coord_approach(pd->x, pd->y, ax+x+w/2, ay+y+h/2, f, delta));
		}

		return 0;
	}

	int ParticleAttractor::draw_debug(double system_x, double system_y, E_RGB color) {
		int ax = get_following_x(system_x);
		int ay = get_following_y(system_y);

		return draw_rectangle(ax+x, ay+y, w, h, false, color);
	}
}

#endif // BEE_RENDER_PARTICLE_ATTRACTOR
