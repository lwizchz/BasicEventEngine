/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_CHANGER_H
#define BEE_RENDER_PARTICLE_CHANGER_H 1

#include "../../enum.hpp"

namespace bee {
	// Forward declarations
	class Instance;
	class Particle;
	class ParticleData;

	class ParticleChanger {
			Instance* following;

			Particle* particle_before;
			Particle* particle_after;

			double x, y;
			unsigned int w, h;

			E_PS_SHAPE shape;

			E_PS_CHANGE change_type;

			double get_following_x(double);
			double get_following_y(double);
		public:
			ParticleChanger();
			ParticleChanger(double, double, unsigned int, unsigned int, Particle*, Particle*);

			int set_following(Instance*);
			int set_part_before(Particle*);
			int set_part_after(Particle*);

			Particle* get_part_before();
			Particle* get_part_after();

			int handle(ParticleData*, double, double);

			int draw_debug(double, double, E_RGB);
	};
}

#endif // BEE_RENDER_PARTICLE_CHANGER_H
