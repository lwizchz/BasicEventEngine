/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_VIEWPORT
#define BEE_RENDER_VIEWPORT 1

#include "viewport.hpp"

#include "../defines.hpp"

#include "../core/enginestate.hpp"
#include "../core/window.hpp"

#include "../render/render.hpp"
#include "../render/renderer.hpp"
#include "../render/shader.hpp"

#include "../resource/texture.hpp"

namespace bee {
	ViewPort::ViewPort() :
		ViewPort(
			true,
			{0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT},
			{0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT}
		)
	{}
	ViewPort::ViewPort(bool _is_active, SDL_Rect _view, SDL_Rect _port) :
		is_active(_is_active),

		view(_view),
		port(_port),

		texture(new Texture()),

		update_func(nullptr),

		shader(nullptr)
	{
		texture->load_as_target(get_window().w, get_window().h);
	}
	ViewPort::~ViewPort() {
		delete texture;
	}

	void ViewPort::update() {
		if (update_func != nullptr) {
			update_func(this);
		}
	}
	void ViewPort::draw() {
		ShaderProgram* s (engine->renderer->program);
		if (shader != nullptr) {
			s = shader;
		}
		s->apply();
		render::set_viewport(nullptr);

		update();
		texture->draw(0, 0, 0);

		glUniform1i(s->get_location("flip"), 2);
		render::render_textures();
		glUniform1i(s->get_location("flip"), 0);
	}
}

#endif // BEE_RENDER_VIEWPORT
