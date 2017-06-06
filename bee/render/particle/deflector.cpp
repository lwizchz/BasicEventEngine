/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_DEFLECTOR
#define _BEE_RENDER_PARTICLE_DEFLECTOR 1

#include "deflector.hpp"

namespace bee {
	ParticleDeflector::ParticleDeflector() :
		x(0.0),
		y(0.0),
		w(1),
		h(1),
		following(nullptr),

		friction(1.0)
	{}
}

#endif // _BEE_RENDER_PARTICLE_DEFLECTOR
