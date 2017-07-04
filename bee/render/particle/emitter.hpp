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
	// Forward declarations
	class Instance;
	class Particle;
	class ParticleData;
	class ParticleSystem;

	class ParticleEmitter {
			Instance* following;

			double x, y;
			unsigned int w, h;

			E_PS_SHAPE shape;
			E_PS_DISTR distribution;

			Particle* particle_type;

			int number;
			int number_count;

			double get_following_x(double);
			double get_following_y(double);

			int emit(ParticleSystem*, double, double);
		public:
			ParticleEmitter();
			ParticleEmitter(double, double, unsigned int, unsigned int, Particle*);

			int set_following(Instance*);
			int set_number(int);

			int handle(ParticleSystem*, double, double);

			int draw_debug(double, double, E_RGB);
	};
}

#endif // BEE_RENDER_PARTICLE_EMITTER_H
