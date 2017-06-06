/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_RENDER_RGBA
#define _BEE_RENDER_RGBA 1

#include "rgba.hpp"

namespace bee {
	RGBA::RGBA() :
		r(0),
		g(0),
		b(0),
		a(0)
	{}
	RGBA::RGBA(Uint8 nr, Uint8 ng, Uint8 nb, Uint8 na) :
		r(nr),
		g(ng),
		b(nb),
		a(na)
	{}
}

#endif // BEE_RENDER_RGBA
