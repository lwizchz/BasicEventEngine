/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_GAMEOPTIONS
#define BEE_INIT_GAMEOPTIONS 1

#include <map>
#include <vector>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

#include "gameoptions.hpp"

#include "../engine.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/window.hpp"

#include "../network/network.hpp"

#include "../render/renderer.hpp"

namespace bee {
	GameOptions::GameOptions() :
		is_fullscreen(true),
		is_borderless(true),
		is_resizable(true),
		is_maximized(true),
		is_highdpi(false),
		is_visible(true),
		is_minimized(false),

		renderer_type(E_RENDERER::OPENGL3),
		is_vsync_enabled(false),
		is_basic_shaders_enabled(false),

		is_network_enabled(true),
		is_debug_enabled(false),

		should_assert(true),
		single_run(false),
		is_headless(false)
	{}
	GameOptions::GameOptions(bool f, bool b, bool r, bool m, bool h, bool v, E_RENDERER rend, bool vsync, bool bs, bool n, bool d) :
		is_fullscreen(f),
		is_borderless(b),
		is_resizable(r),
		is_maximized(m),
		is_highdpi(h),
		is_visible(v),
		is_minimized(false),

		renderer_type(rend),
		is_vsync_enabled(vsync),
		is_basic_shaders_enabled(bs),

		is_network_enabled(n),
		is_debug_enabled(d),

		should_assert(true),
		single_run(false),
		is_headless(false)
	{}

	const GameOptions& get_options() {
		return *(engine->options);
	}
	int set_options(const GameOptions& new_options) {
		// Change fullscreen state
		if (engine->options->is_fullscreen != new_options.is_fullscreen) {
			engine->options->is_fullscreen = new_options.is_fullscreen;

			Uint32 f = 0;
			if (engine->options->is_fullscreen) {
				f = SDL_WINDOW_FULLSCREEN_DESKTOP;
			}
			SDL_SetWindowFullscreen(engine->renderer->window, f);
		}
		// Change borderless state
		if (engine->options->is_borderless != new_options.is_borderless) {
			engine->options->is_borderless = new_options.is_borderless;

			SDL_bool b = SDL_TRUE;
			if (engine->options->is_borderless) {
				b = SDL_FALSE;
			}
			SDL_SetWindowBordered(engine->renderer->window, b);
		}
		// Change resizable state
		if (engine->options->is_resizable != new_options.is_resizable) {
			engine->options->is_resizable = new_options.is_resizable;

			if (engine->options->is_resizable) {
				SDL_SetWindowMaximumSize(engine->renderer->window, 16384, 16384);
				SDL_SetWindowMinimumSize(engine->renderer->window, 128, 128);
			} else {
				SDL_SetWindowMaximumSize(engine->renderer->window, get_width(), get_height());
				SDL_SetWindowMinimumSize(engine->renderer->window, get_width(), get_height());
			}
		}
		// Change maximized state
		if (engine->options->is_maximized != new_options.is_maximized) {
			engine->options->is_maximized = new_options.is_maximized;

			if (engine->options->is_maximized) {
				SDL_MaximizeWindow(engine->renderer->window);
			} else {
				if (engine->options->is_resizable) {
					SDL_RestoreWindow(engine->renderer->window);
				} else {
					SDL_MinimizeWindow(engine->renderer->window);
				}
			}
		}
		// Change highdpi state
		if (engine->options->is_highdpi != new_options.is_highdpi) {
			engine->options->is_highdpi = new_options.is_highdpi;

			// I currently have no way to test highdpi functionality
		}
		// Change visible state
		if (engine->options->is_visible != new_options.is_visible) {
			engine->options->is_visible = new_options.is_visible;

			if (engine->options->is_visible) {
				SDL_ShowWindow(engine->renderer->window);
			} else {
				SDL_HideWindow(engine->renderer->window);
			}
		}
		// Change minimization state
		if (engine->options->is_minimized != new_options.is_minimized) {
			engine->options->is_minimized = new_options.is_minimized;

			if (engine->options->is_minimized) {
				SDL_MinimizeWindow(engine->renderer->window);
			} else {
				SDL_RestoreWindow(engine->renderer->window);
			}
		}
		// Change OpenGL state
		if (engine->options->renderer_type != new_options.renderer_type) {
			engine->options->renderer_type = new_options.renderer_type;

			if (engine->options->renderer_type != E_RENDERER::SDL) { // Enter OpenGL mode
				engine->renderer->sdl_renderer_close();
				engine->renderer->render_reset();
			} else { // Enter SDL rendering mode
				engine->renderer->opengl_close();
				engine->renderer->render_reset();
			}
		}
		// Change vsync state
		if (engine->options->is_vsync_enabled != new_options.is_vsync_enabled) {
			engine->options->is_vsync_enabled = new_options.is_vsync_enabled;

			engine->renderer->render_reset();
		}
		// Change networking state
		if (engine->options->is_network_enabled != new_options.is_network_enabled) {
			engine->options->is_network_enabled = new_options.is_network_enabled;

			if (engine->options->is_network_enabled) {
				net::init();
			} else {
				net::close();
			}
		}
		// Change debugging state
		if (engine->options->is_debug_enabled != new_options.is_debug_enabled) {
			engine->options->is_debug_enabled = new_options.is_debug_enabled;
		}

		// Refuse to modify should_assert
		if (engine->options->should_assert != new_options.should_assert) {
			messenger::send({"engine", "options"}, E_MESSAGE::WARNING, "Cannot modify should_assert GameOption after initialization");
		}
		// Refuse to modify single_run
		if (engine->options->single_run != new_options.single_run) {
			messenger::send({"engine", "options"}, E_MESSAGE::WARNING, "Cannot modify single_run GameOption after initialization");
		}
		// Refuse to modify is_headless
		if (engine->options->is_headless != new_options.is_headless) {
			messenger::send({"engine", "options"}, E_MESSAGE::WARNING, "Cannot modify is_headless GameOption after initialization");
		}

		return 0;
	}
}

#endif // BEE_INIT_GAMEOPTIONS
