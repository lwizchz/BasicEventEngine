/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_ENGINESTATE
#define BEE_CORE_ENGINESTATE 1

#include <functional>
#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "enginestate.hpp" // Include the engine headers

#include "../enum.hpp"

#include "../init/gameoptions.hpp"

#include "console.hpp"

#include "../network/network.hpp"
#include "../network/connection.hpp"

#include "../render/renderer.hpp"
#include "../render/rgba.hpp"

#include "../resource/texture.hpp"
#include "../resource/font.hpp"
#include "../resource/room.hpp"

namespace bee {
	EngineState::EngineState(int _argc, char** _argv, GameOptions* _options) :
		argc(_argc),
		argv(_argv),
		flags(),
		options(_options),

		quit(false),
		is_ready(false),
		is_paused(false),

		first_room(nullptr),
		current_room(nullptr),

		width(DEFAULT_WINDOW_WIDTH),
		height(DEFAULT_WINDOW_HEIGHT),
		cursor(nullptr),

		renderer(new Renderer()),

		color(new RGBA()),
		font_default(nullptr),

		has_mouse(false),
		has_focus(false),

		tickstamp(0),
		fps_ticks(0),
		tick_delta(0),

		volume(1.0),

		fps_goal(DEFAULT_GAME_FPS),
		fps_max(300),
		fps_unfocused(fps_max/20),
		fps_count(0),
		frame_number(0),

		texture_before(nullptr),
		texture_after(nullptr),
		transition_type(E_TRANSITION::NONE),
		transition_speed(1024.0/DEFAULT_GAME_FPS),
		transition_custom_func(nullptr),

		keystate(nullptr),
		keystrings_keys(),
		keystrings_strings(),

		commandline_input(),
		commandline_current(0),

		fps_stable(0)
	{}
	EngineState::~EngineState() {
		free();
	}

	int EngineState::free() {
		if (font_default != nullptr) {
			delete font_default;
			font_default = nullptr;
		}

		if (texture_before != nullptr) {
			texture_before->free();
			delete texture_before;
			texture_before = nullptr;
		}
		if (texture_after != nullptr) {
			texture_after->free();
			delete texture_after;
			texture_after = nullptr;
		}

		if (color != nullptr) {
			delete color;
			color = nullptr;
		}

		if (renderer != nullptr) {
			delete renderer;
			renderer = nullptr;
		}
		if (cursor != nullptr) {
			SDL_FreeCursor(cursor);
			cursor = nullptr;
		}

		return 0;
	}
}

#endif // BEE_CORE_ENGINESTATE
