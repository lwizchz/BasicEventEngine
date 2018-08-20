/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_BACKGROUND
#define BEE_RENDER_BACKGROUND 1

#include "background.hpp"

namespace bee {
	/**
	* Construct the data struct and initiliaze all values.
	*/
	Background::Background() :
		texture(nullptr),
		is_visible(false),
		is_foreground(false),
		transform()
	{}
	/**
	* Construct the data struct and initiliaze with all the given values.
	* @param _texture the Texture to use
	* @param _is_visible whether the Background should be drawn or not
	* @param _is_foreground whether the Background should be drawn above or below the Instances
	* @param _transform the TextureTransform data to use when rendering
	*/
	Background::Background(Texture* _texture, bool _is_visible, bool _is_foreground, TextureTransform _transform) :
		texture(_texture),
		is_visible(_is_visible),
		is_foreground(_is_foreground),
		transform(_transform)
	{}
}

#endif // BEE_RENDER_BACKGROUND
