/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
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
	ParticleAttractor::ParticleAttractor(double x, double y, unsigned int w, unsigned int h) :
		region(nullptr, x, y, w, h),

		force(50.0),
		max_distance(100.0),
		force_type(E_PS_FORCE::LINEAR)
	{}

	int ParticleAttractor::set_following(Instance* inst) {
		region.following = inst;
		return 0;
	}
	int ParticleAttractor::set_force(double _force) {
		force = _force;
		return 0;
	}
	int ParticleAttractor::set_max_distance(double _max_distance) {
		max_distance = _max_distance;
		return 0;
	}

	double ParticleAttractor::get_following_x(double default_x) {
		if (region.following != nullptr) {
			return region.following->get_x();
		}
		return default_x;
	}
	double ParticleAttractor::get_following_y(double default_y) {
		if (region.following != nullptr) {
			return region.following->get_y();
		}
		return default_y;
	}

	int ParticleAttractor::handle(ParticleData* pd, double system_x, double system_y, double delta) {
		double ax = get_following_x(system_x);
		double ay = get_following_y(system_y);

		int rel_x = ax + region.x + region.w/2;
		int rel_y = ay + region.y + region.h/2;

		double ds = util::dist_sqr(pd->x, pd->y, rel_x, rel_y);
		if (ds < util::sqr(max_distance)) {
			double f = 0.0;
			switch (force_type) {
				case E_PS_FORCE::CONSTANT: {
					f = force * pd->get_deviation_percent();
					break;
				}
				case E_PS_FORCE::LINEAR:
				default: {
					f = force * pd->get_deviation_percent() * (util::sqr(max_distance) - ds) / util::sqr(max_distance);
					break;
				}
				case E_PS_FORCE::QUADRATIC: {
					f = force * pd->get_deviation_percent() * util::sqr((util::sqr(max_distance) - ds) / util::sqr(max_distance));
					break;
				}
			}

			pd->set_position(util::coord_approach(pd->x, pd->y, rel_x, rel_y, f, delta));
		}

		return 0;
	}

	int ParticleAttractor::draw_debug(double system_x, double system_y, E_RGB color) {
		double ax = get_following_x(system_x);
		double ay = get_following_y(system_y);

		return draw_rectangle(static_cast<int>(ax + region.x), static_cast<int>(ay + region.y), region.w, region.h, 1, RGBA(color));
	}
}

#endif // BEE_RENDER_PARTICLE_ATTRACTOR
