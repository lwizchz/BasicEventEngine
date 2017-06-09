/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_DESTROYER_H
#define BEE_RENDER_PARTICLE_DESTROYER_H 1

#include "../../enum.hpp"

namespace bee {
	class Instance;

	class ParticleDestroyer {
		public:
			double x, y;
			unsigned int w, h;
			Instance* following;

			E_PS_SHAPE shape;

			ParticleDestroyer();
	};
}

#endif // BEE_RENDER_PARTICLE_DESTROYER_H
