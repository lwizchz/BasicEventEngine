/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_ATTRACTOR_H
#define _BEE_RENDER_PARTICLE_ATTRACTOR_H 1

#include "../../game.hpp"

class BEE::ParticleAttractor {
	public:
		double x, y;
		unsigned int w, h;
		Instance* following;

		double force;
		double max_distance;
		bee::E_PS_FORCE force_type;

		ParticleAttractor();
};

#endif // _BEE_RENDER_PARTICLE_ATTRACTOR_H
