/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_DESTROYER
#define _BEE_RENDER_PARTICLE_DESTROYER 1

#include "destroyer.hpp"

BEE::ParticleDestroyer::ParticleDestroyer() :
	x(0.0),
	y(0.0),
	w(1),
	h(1),
	following(nullptr),
	
	shape(BEE_PS_SHAPE_RECTANGLE)
{}

#endif // _BEE_RENDER_PARTICLE_DESTROYER
