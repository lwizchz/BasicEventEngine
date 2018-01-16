/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_VIEWPORT_H
#define BEE_RENDER_VIEWPORT_H 1

#include <functional>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	class Texture;
	class ShaderProgram;

	struct ViewPort {
		bool is_active;

		SDL_Rect view;
		SDL_Rect port;

		Texture* texture;

		std::function<void (ViewPort*)> update_func;

		ShaderProgram* shader;

		ViewPort();
		ViewPort(bool, SDL_Rect, SDL_Rect);
		~ViewPort();

		void update();
		void draw();
	};
}

#endif // BEE_RENDER_VIEWPORT_H
