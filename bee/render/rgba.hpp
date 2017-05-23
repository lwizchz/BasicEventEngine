/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_RGBA_H
#define _BEE_RENDER_RGBA_H 1

#include "../game.hpp"

struct BEE::RGBA {
	Uint8 r, g, b, a;

	RGBA();
	RGBA(Uint8, Uint8, Uint8, Uint8);
};

#endif // _BEE_RENDER_RGBA_H
