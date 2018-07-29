/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENGINE_H
#define BEE_ENGINE_H 1

// Include the required library headers
#include <list>

#include <SDL2/SDL.h> // Include the required SDL headers

// User defined in resources/resources.hpp
int init_resources();
int close_resources();

namespace bee { // The engine namespace
	// Forward declarations
	struct ProgramFlag;
	struct GameOption;
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

	// bee/engine.cpp
	int init(int, char**, Room**, const std::list<ProgramFlag*>&, const std::list<GameOption>&);
	int loop();
	int close();

	Uint32 get_ticks();
	Uint32 get_seconds();
	Uint32 get_frame();
	double get_delta();
	Uint32 get_tick_delta();
	unsigned int get_fps_goal();

	void restart_game();
	void end_game();
}

#endif // BEE_ENGINE_H
