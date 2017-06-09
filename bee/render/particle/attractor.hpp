/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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

	class ParticleAttractor {
		public:
			double x, y;
			unsigned int w, h;
			Instance* following;

			double force;
			double max_distance;
			E_PS_FORCE force_type;

			ParticleAttractor();
	};
}

#endif // BEE_RENDER_PARTICLE_ATTRACTOR_H
