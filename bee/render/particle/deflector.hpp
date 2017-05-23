/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_DEFLECTOR_H
#define _BEE_RENDER_PARTICLE_DEFLECTOR_H 1

#include "../../game.hpp"

class BEE::ParticleDeflector {
	public:
		double x, y;
		unsigned int w, h;
		Instance* following;

		double friction;

		ParticleDeflector();
};

#endif // _BEE_RENDER_PARTICLE_DEFLECTOR_H
