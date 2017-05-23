/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_INIT_GAMEOPTIONS
#define _BEE_INIT_GAMEOPTIONS 1

#include "gameoptions.hpp"

BEE::GameOptions::GameOptions() :
	is_fullscreen(true),
	is_borderless(true),
	is_resizable(true),
	is_maximized(true),
	is_highdpi(false),
	is_visible(true),
	is_minimized(false),

	renderer_type(BEE_RENDERER_OPENGL3),
	is_vsync_enabled(false),
	is_basic_shaders_enabled(false),

	is_network_enabled(true),
	is_debug_enabled(false),

	should_assert(true),
	single_run(false)
{}
BEE::GameOptions::GameOptions(bool f, bool b, bool r, bool m, bool h, bool v, bee_renderer_t rend, bool vsync, bool bs, bool n, bool d) :
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

BEE::GameOptions BEE::get_options() const {
	return *options;
}
int BEE::set_options(const GameOptions& new_options) {
	if (options->is_fullscreen != new_options.is_fullscreen) {
		// Change fullscreen state
		options->is_fullscreen = new_options.is_fullscreen;

		bool b = 0;
		if (options->is_fullscreen) {
			b = SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		SDL_SetWindowFullscreen(window, b);
	}
	if (options->is_borderless != new_options.is_borderless) {
		// Change borderless state
		options->is_borderless = new_options.is_borderless;

		SDL_bool b = SDL_TRUE;
		if (options->is_borderless) {
			b = SDL_FALSE;
		}
		SDL_SetWindowBordered(window, b);
	}
	if (options->is_resizable != new_options.is_resizable) {
		// Change resizable state
		options->is_resizable = new_options.is_resizable;

		if (options->is_resizable) {
			SDL_SetWindowMaximumSize(window, 16384, 16384);
			SDL_SetWindowMinimumSize(window, 128, 128);
		} else {
			SDL_SetWindowMaximumSize(window, get_width(), get_height());
			SDL_SetWindowMinimumSize(window, get_width(), get_height());
		}
	}
	if (options->is_maximized != new_options.is_maximized) {
		// Change maximized state
		options->is_maximized = new_options.is_maximized;

		if (options->is_maximized) {
			SDL_MaximizeWindow(window);
		} else {
			if (options->is_resizable) {
				SDL_RestoreWindow(window);
			} else {
				SDL_MinimizeWindow(window);
			}
		}
	}
	if (options->is_highdpi != new_options.is_highdpi) {
		// Change highdpi state
		options->is_highdpi = new_options.is_highdpi;

		// I currently have no way to test highdpi functionality
	}
	if (options->is_visible != new_options.is_visible) {
		// Change visible state
		options->is_visible = new_options.is_visible;

		if (options->is_visible) {
			SDL_ShowWindow(window);
		} else {
			SDL_HideWindow(window);
		}
	}
	if (options->is_minimized != new_options.is_minimized) {
		// Change minimization state
		options->is_minimized = new_options.is_minimized;

		if (options->is_minimized) {
			SDL_MinimizeWindow(window);
		} else {
			SDL_RestoreWindow(window);
		}
	}
	if (options->renderer_type != new_options.renderer_type) {
		// Change OpenGL state
		options->renderer_type = new_options.renderer_type;

		if (options->renderer_type != BEE_RENDERER_SDL) { // Enter OpenGL mode
			sdl_renderer_close();
			render_reset();
		} else { // Enter SDL rendering mode
			opengl_close();
			render_reset();
		}
	}
	if (options->is_vsync_enabled != new_options.is_vsync_enabled) {
		// Change vsync state
		options->is_vsync_enabled = new_options.is_vsync_enabled;

		render_reset();
	}
	if (options->is_network_enabled != new_options.is_network_enabled) {
		// Change networking state
		options->is_network_enabled = new_options.is_network_enabled;

		if (options->is_network_enabled) {
			net_init();
		} else {
			net_close();
		}
	}
	if (options->is_debug_enabled != new_options.is_debug_enabled) {
		// Change debugging state
		options->is_debug_enabled = new_options.is_debug_enabled;
	}

	return 0;
}

#endif // _BEE_INIT_GAMEOPTIONS
