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
	class Instance;
	class Particle;

	class ParticleChanger {
		public:
			double x, y;
			unsigned int w, h;
			Instance* following;

			E_PS_SHAPE shape;

			Particle* particle_before;
			Particle* particle_after;

			E_PS_CHANGE change_type;

			ParticleChanger();
	};
}

#endif // BEE_RENDER_PARTICLE_CHANGER_H
