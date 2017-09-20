/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENGINE_H
#define BEE_ENGINE_H 1

// Include the required library headers
#include <string>
#include <functional>
#include <memory>
#include <list>
#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "defines.hpp"

#include <glm/glm.hpp> // Include the required OpenGL headers

#include "enum.hpp" // Include the required engine enumerations

namespace bee { // The engine namespace
	// Forward declarations
	struct ProgramFlag;
	struct GameOptions;
	class Room;

	struct EngineState; // The struct which holds the main game state

	extern EngineState* engine;
	extern bool is_initialized;

	namespace internal {
		int init_sdl();
		int handle_sdl_events();
		int handle_drawing();
		int close_sdl();

		int update_delta();
		int frame_delay();
	}

	// User defined in resources/resources.hpp
	int init_resources();
	int close_resources();

	// bee/engine.cpp
	int init(int, char**, const std::list<ProgramFlag*>&, Room**, GameOptions*);
	int loop();
	int close();

	Uint32 get_ticks();
	Uint32 get_seconds();
	Uint32 get_frame();
	double get_delta();
	Uint32 get_tick_delta();
	unsigned int get_fps_goal();

	int restart_game();
	int end_game();
}

#endif // BEE_ENGINE_H
