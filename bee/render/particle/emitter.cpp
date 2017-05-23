/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_EMITTER
#define _BEE_RENDER_PARTICLE_EMITTER 1

#include "emitter.hpp"

BEE::ParticleEmitter::ParticleEmitter() :
	x(0.0),
	y(0.0),
	w(1),
	h(1),
	following(nullptr),
	
	shape(BEE_PS_SHAPE_RECTANGLE),
	distribution(BEE_PS_DISTR_LINEAR),
	
	particle_type(nullptr),
	
	number(1),
	number_count(0)
{}

#endif // _BEE_RENDER_PARTICLE_EMITTER
