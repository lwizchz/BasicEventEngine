/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_DESTROYER_H
#define BEE_RENDER_PARTICLE_DESTROYER_H 1

#include "../../enum.hpp"

#include "region.hpp"

namespace bee {
	// Forward declarations
	class ParticleData;

	class ParticleDestroyer {
			ParticleRegion region;

			E_PS_SHAPE shape;

			double get_following_x(double);
			double get_following_y(double);
		public:
			ParticleDestroyer(double, double, unsigned int, unsigned int);

			int set_following(Instance*);

			bool handle(ParticleData*, double, double);

			int draw_debug(double, double, E_RGB);
	};
}

#endif // BEE_RENDER_PARTICLE_DESTROYER_H
