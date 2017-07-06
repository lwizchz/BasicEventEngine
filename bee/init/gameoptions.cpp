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
		single_run(false)
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
		single_run(false)
	{}

	GameOptions get_options() {
		return *(engine->options);
	}
	int set_options(const GameOptions& new_options) {
		if (engine->options->is_fullscreen != new_options.is_fullscreen) {
			// Change fullscreen state
			engine->options->is_fullscreen = new_options.is_fullscreen;

			Uint32 f = 0;
			if (engine->options->is_fullscreen) {
				f = SDL_WINDOW_FULLSCREEN_DESKTOP;
			}
			SDL_SetWindowFullscreen(engine->renderer->window, f);
		}
		if (engine->options->is_borderless != new_options.is_borderless) {
			// Change borderless state
			engine->options->is_borderless = new_options.is_borderless;

			SDL_bool b = SDL_TRUE;
			if (engine->options->is_borderless) {
				b = SDL_FALSE;
			}
			SDL_SetWindowBordered(engine->renderer->window, b);
		}
		if (engine->options->is_resizable != new_options.is_resizable) {
			// Change resizable state
			engine->options->is_resizable = new_options.is_resizable;

			if (engine->options->is_resizable) {
				SDL_SetWindowMaximumSize(engine->renderer->window, 16384, 16384);
				SDL_SetWindowMinimumSize(engine->renderer->window, 128, 128);
			} else {
				SDL_SetWindowMaximumSize(engine->renderer->window, get_width(), get_height());
				SDL_SetWindowMinimumSize(engine->renderer->window, get_width(), get_height());
			}
		}
		if (engine->options->is_maximized != new_options.is_maximized) {
			// Change maximized state
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
		if (engine->options->is_highdpi != new_options.is_highdpi) {
			// Change highdpi state
			engine->options->is_highdpi = new_options.is_highdpi;

			// I currently have no way to test highdpi functionality
		}
		if (engine->options->is_visible != new_options.is_visible) {
			// Change visible state
			engine->options->is_visible = new_options.is_visible;

			if (engine->options->is_visible) {
				SDL_ShowWindow(engine->renderer->window);
			} else {
				SDL_HideWindow(engine->renderer->window);
			}
		}
		if (engine->options->is_minimized != new_options.is_minimized) {
			// Change minimization state
			engine->options->is_minimized = new_options.is_minimized;

			if (engine->options->is_minimized) {
				SDL_MinimizeWindow(engine->renderer->window);
			} else {
				SDL_RestoreWindow(engine->renderer->window);
			}
		}
		if (engine->options->renderer_type != new_options.renderer_type) {
			// Change OpenGL state
			engine->options->renderer_type = new_options.renderer_type;

			if (engine->options->renderer_type != E_RENDERER::SDL) { // Enter OpenGL mode
				engine->renderer->sdl_renderer_close();
				engine->renderer->render_reset();
			} else { // Enter SDL rendering mode
				engine->renderer->opengl_close();
				engine->renderer->render_reset();
			}
		}
		if (engine->options->is_vsync_enabled != new_options.is_vsync_enabled) {
			// Change vsync state
			engine->options->is_vsync_enabled = new_options.is_vsync_enabled;

			engine->renderer->render_reset();
		}
		if (engine->options->is_network_enabled != new_options.is_network_enabled) {
			// Change networking state
			engine->options->is_network_enabled = new_options.is_network_enabled;

			if (engine->options->is_network_enabled) {
				net_init();
			} else {
				net_close();
			}
		}
		if (engine->options->is_debug_enabled != new_options.is_debug_enabled) {
			// Change debugging state
			engine->options->is_debug_enabled = new_options.is_debug_enabled;
		}

		return 0;
	}
}

#endif // BEE_INIT_GAMEOPTIONS
