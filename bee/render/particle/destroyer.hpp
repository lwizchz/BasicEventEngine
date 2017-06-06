/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_DESTROYER_H
#define _BEE_RENDER_PARTICLE_DESTROYER_H 1

#include "../../game.hpp"

class BEE::ParticleDestroyer {
	public:
		double x, y;
		unsigned int w, h;
		Instance* following;

		bee::E_PS_SHAPE shape;

		ParticleDestroyer();
};

#endif // _BEE_RENDER_PARTICLE_DESTROYER_H
