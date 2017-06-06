/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_CHANGER_H
#define _BEE_RENDER_PARTICLE_CHANGER_H 1

#include "../../game.hpp"

class BEE::ParticleChanger {
	public:
		double x, y;
		unsigned int w, h;
		Instance* following;

		bee::E_PS_SHAPE shape;

		Particle* particle_before;
		Particle* particle_after;

		bee::E_PS_CHANGE change_type;

		ParticleChanger();
};

#endif // _BEE_RENDER_PARTICLE_CHANGER_H
