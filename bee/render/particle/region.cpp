/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_PARTICLE_REGION
#define BEE_RENDER_PARTICLE_REGION 1

#include "region.hpp"

#include "../../core/instance.hpp"

namespace bee {
	ParticleRegion::ParticleRegion() :
		ParticleRegion(nullptr, 0.0, 0.0, 1, 1)
	{}
	ParticleRegion::ParticleRegion(Instance* _following, double _x, double _y, unsigned int _w, unsigned int _h) :
		following(_following),
		x(_x), y(_y),
		w(_w), h(_h)
	{
		w = std::max(w, 1u);
		h = std::max(h, 1u);
	}
}

#endif // BEE_RENDER_PARTICLE_REGION
