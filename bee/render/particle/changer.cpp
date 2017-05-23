/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_PARTICLE_CHANGER
#define _BEE_RENDER_PARTICLE_CHANGER 1

#include "changer.hpp"

BEE::ParticleChanger::ParticleChanger() :
	x(0.0),
	y(0.0),
	w(1), h(1),
	following(nullptr),
	
	shape(BEE_PS_SHAPE_RECTANGLE),
	
	particle_before(nullptr),
	particle_after(nullptr),
	
	change_type(BEE_PS_CHANGE_ALL)
{}

#endif // _BEE_RENDER_PARTICLE_CHANGER
