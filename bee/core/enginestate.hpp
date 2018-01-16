/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_ENGINESTATE_H
#define BEE_CORE_ENGINESTATE_H 1

#include <sstream>
#include <list>
#include <memory>
#include <functional>
#include <map>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	struct ProgramFlag;
	struct GameOptions;

	class Messenger;
	class Console;
	struct NetworkConnection;

	class Renderer;
	struct RGBA;

	class Texture;
	class Font;
	class Room;

	struct EngineState {
		// These contain data about the engine initialization
		int argc;
		char** argv; // The provided commandline flags
		std::list<ProgramFlag*> flags; // The available commandline flags
		GameOptions* options; // The engine options

		// These contain data about the event loop
		bool quit, is_ready, is_paused;
		Room *first_room, *current_room;

		unsigned int width, height;

		Renderer* renderer;

		// This is the current drawing color
		RGBA* color;
		Font* font_default; // A default font for engine drawing

		// These contain data about the current window state
		bool has_mouse, has_focus;

		Uint32 tickstamp, fps_ticks, tick_delta;

		double volume;

		unsigned int fps_goal, fps_max, fps_unfocused;
		unsigned int fps_count;
		Uint32 frame_number;
		Texture* texture_before;
		Texture* texture_after;
		E_TRANSITION transition_type;
		double transition_speed;
		std::function<void (Texture*, Texture*)> transition_custom_func;

		std::vector<std::string> commandline_input;
		unsigned int commandline_current;

		unsigned int fps_stable;

		EngineState(int, char**, GameOptions*);
		~EngineState();

		int free(); // Must be called to free data before closing SDL
	};

	extern EngineState* engine;
}

#endif // BEE_CORE_ENGINESTATE_H
