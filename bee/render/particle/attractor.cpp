/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_ATTRACTOR
#define BEE_RENDER_PARTICLE_ATTRACTOR 1

#include "attractor.hpp"

namespace bee {
	ParticleAttractor::ParticleAttractor() :
		x(0.0),
		y(0.0),
		w(1),
		h(1),
		following(nullptr),

		force(1.0),
		max_distance(100.0),
		force_type(E_PS_FORCE::LINEAR)
	{}
}

#endif // BEE_RENDER_PARTICLE_ATTRACTOR
