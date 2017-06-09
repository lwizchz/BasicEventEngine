/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_EMITTER_H
#define BEE_RENDER_PARTICLE_EMITTER_H 1

#include "../../enum.hpp"

namespace bee {
	class Instance;
	class Particle;

	class ParticleEmitter {
		public:
			double x, y;
			unsigned int w, h;
			Instance* following;

			E_PS_SHAPE shape;
			E_PS_DISTR distribution;

			Particle* particle_type;

			int number;
			int number_count;

			ParticleEmitter();
	};
}

#endif // BEE_RENDER_PARTICLE_EMITTER_H
