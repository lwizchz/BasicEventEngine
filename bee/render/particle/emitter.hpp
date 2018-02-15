/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_EMITTER_H
#define BEE_RENDER_PARTICLE_EMITTER_H 1

#include "../../enum.hpp"

#include "region.hpp"

namespace bee {
	// Forward declarations
	class Particle;
	class ParticleData;
	class ParticleSystem;

	class ParticleEmitter {
			ParticleRegion region;

			E_PS_SHAPE shape;
			E_PS_DISTR distribution;

			Particle* particle_type;

			int number;
			int number_count;

			double get_following_x(double);
			double get_following_y(double);

			int emit(ParticleSystem*, double, double);
		public:
			ParticleEmitter(double, double, unsigned int, unsigned int, Particle*);

			int set_following(Instance*);
			int set_number(int);

			int handle(ParticleSystem*, double, double);

			int draw_debug(double, double, E_RGB);
	};
}

#endif // BEE_RENDER_PARTICLE_EMITTER_H
