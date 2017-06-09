/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_CHANGER
#define BEE_RENDER_PARTICLE_CHANGER 1

#include "changer.hpp"

namespace bee {
	ParticleChanger::ParticleChanger() :
		x(0.0),
		y(0.0),
		w(1), h(1),
		following(nullptr),

		shape(E_PS_SHAPE::RECTANGLE),

		particle_before(nullptr),
		particle_after(nullptr),

		change_type(E_PS_CHANGE::ALL)
	{}
}

#endif // BEE_RENDER_PARTICLE_CHANGER
