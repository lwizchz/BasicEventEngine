/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_DEFLECTOR_H
#define BEE_RENDER_PARTICLE_DEFLECTOR_H 1

namespace bee {
	class Instance;

	class ParticleDeflector {
		public:
			double x, y;
			unsigned int w, h;
			Instance* following;

			double friction;

			ParticleDeflector();
	};
}

#endif // BEE_RENDER_PARTICLE_DEFLECTOR_H
