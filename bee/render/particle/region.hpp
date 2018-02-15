/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_REGION_H
#define BEE_RENDER_PARTICLE_REGION_H 1

namespace bee {
	// Forward declaration
	class Instance;

	struct ParticleRegion {
		Instance* following;

		double x, y;
		unsigned int w, h;

		ParticleRegion();
		ParticleRegion(Instance*, double, double, unsigned int, unsigned int);
	};
}

#endif // BEE_RENDER_PARTICLE_REGION_H
