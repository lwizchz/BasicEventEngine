/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_RGBA_H
#define BEE_RENDER_RGBA_H 1

#include <SDL2/SDL.h>

namespace bee {
	struct RGBA {
		Uint8 r, g, b, a;

		RGBA();
		RGBA(int, int, int, int);
	};
}

#endif // BEE_RENDER_RGBA_H
