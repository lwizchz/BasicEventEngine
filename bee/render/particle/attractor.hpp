/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_ATTRACTOR_H
#define BEE_RENDER_PARTICLE_ATTRACTOR_H 1

#include "../../enum.hpp"

namespace bee {
	// Forward declaration
	class Instance;
	class ParticleData;

	class ParticleAttractor {
			Instance* following;

			double x, y;
			unsigned int w, h;

			double force;
			double max_distance;
			E_PS_FORCE force_type;

			double get_following_x(double);
			double get_following_y(double);
		public:
			ParticleAttractor();
			ParticleAttractor(double, double, unsigned int, unsigned int);

			int set_following(Instance*);
			int set_force(double);
			int set_max_distance(double);

			int handle(ParticleData*, double, double, double);

			int draw_debug(double, double, E_RGB);
	};
}

#endif // BEE_RENDER_PARTICLE_ATTRACTOR_H
