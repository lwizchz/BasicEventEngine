/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_BACKGROUND_H
#define BEE_RENDER_BACKGROUND_H 1

#include "../resource/texture.hpp"

namespace bee {
	struct Background { // The data struct which is used to pass data to the Room class
		Texture* texture; // A pointer to the texture to use this data with
		bool is_visible; // Whether to draw the background
		bool is_foreground; // Whether to draw the texture above or below the other sprites
		TextureTransform transform; // The data to transform the texture with

		// See bee/render/background.cpp for function comments
		Background();
		Background(Texture*, bool, bool, TextureTransform);
	};
}

#endif // BEE_RENDER_BACKGROUND_H
