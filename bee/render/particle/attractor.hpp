/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_ATTRACTOR_H
#define BEE_RENDER_PARTICLE_ATTRACTOR_H 1

#include "../../enum.hpp"

#include "region.hpp"

namespace bee {
	// Forward declaration
	class ParticleData;

	class ParticleAttractor {
			ParticleRegion region;

			double force;
			double max_distance;
			E_PS_FORCE force_type;

			double get_following_x(double);
			double get_following_y(double);
		public:
			ParticleAttractor(double, double, unsigned int, unsigned int);

			int set_following(Instance*);
			int set_force(double);
			int set_max_distance(double);

			int handle(ParticleData*, double, double, double);

			int draw_debug(double, double, E_RGB);
	};
}

#endif // BEE_RENDER_PARTICLE_ATTRACTOR_H
