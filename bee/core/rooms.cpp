/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_ROOMS
#define BEE_CORE_ROOMS 1

#include "../defines.hpp"

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "rooms.hpp"

#include "../util/collision.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "enginestate.hpp"
#include "window.hpp"

#include "../render/camera.hpp"
#include "../render/render.hpp"
#include "../render/renderer.hpp"
#include "../render/shader.hpp"
#include "../render/transition.hpp"

#include "../resource/room.hpp"

namespace bee {
	/**
	* Change to the current room again.
	* @note An exception is thrown in order to jump to the end of the event loop.
	* @throws int(3) Jump to loop end and reload the room
	*/
	void restart_room() {
		throw 3;
	}
	/**
	* Handle room transitions and resource changes between rooms.
	* @note Currently transitions are drawn even if none is set because without it nothing will draw for some reason.
	*
	* @param new_room the room to change to
	* @param should_jump whether to should jump to the end of the event loop when finished setting up the new room
	*
	* @retval 0 success
	* @throws int(0) Jump to loop end and continue
	*/
	int change_room(Room* new_room, bool should_jump) {
		if (((engine->quit)^(new_room != nullptr)) == false) { // Abort the room change if either the quit flag is set without a null room or if the quit flag is unset with a null room
			throw 0; // Throw an exception to jump to the end of the event loop
			return 0;
		}

		bool is_game_start = false;
		if (engine->current_room != nullptr) { // if we are currently in a room
			if (engine->transition_type != E_TRANSITION::NONE) { // If a transition has been defined then draw the current room into the before buffer
				render::set_target(engine->texture_before);
				engine->current_room->draw();
			}
			if (new_room != nullptr) {
				engine->current_room->room_end(); // Run the room_end event for the current room
				engine->current_room->reset_properties(); // Reset the current room's properties
			}
		} else { // if we are not in a room
			if (engine->transition_type != E_TRANSITION::NONE) { // If a transition has been defined then draw nothing into the before buffer
				render::set_target(engine->texture_before);
				render::clear();
				render::render();
			}
			is_game_start = true;
			engine->first_room = new_room; // Set the new room as the first room
		}

		if (new_room == nullptr) { // If we're transitioning to a null room, i.e. the game is ending
			if (engine->transition_type != E_TRANSITION::NONE) { // If a transition has been defined then prepare for drawing an empty room into the after buffer
				render::set_target(engine->texture_after);
				render::clear();
				render::render();
				render::reset_target();
				draw_transition(); // Animate the defined transition from the before and after buffers
			}
			engine->quit = true; // Set the quit flag just in case this was called in the main loop
			return 0;
		}

		const Room* old_room = engine->current_room;
		engine->current_room = new_room; // Set the new room as the current room
		engine->is_ready = false; // Set the event loop as not running
		engine->current_room->reset_properties(); // Reset the new room's properties
		engine->current_room->transfer_instances(old_room); // Transfer the persistent instance from the previous room
		engine->current_room->init(); // Initialize the room

		//set_window_title(engine->current_room->get_name()); // Set the window title to the room's name
		messenger::send({"engine", "room"}, E_MESSAGE::INFO, "Changed to room \"" + engine->current_room->get_name() + "\"");

		if (engine->transition_type != E_TRANSITION::NONE) { // If a transition has been defined then prepare for drawing the new room into the after buffer
			render::set_target(engine->texture_after);
		} else { // Otherwise reset the render target just to be sure
			render::reset_target();
		}
		if (!get_option("is_headless").i) {
			render::clear();
		}

		engine->is_ready = true; // Set the event loop as running
		messenger::handle();
		engine->current_room->create(); // Run the create event for the new room
		if (is_game_start) { // If this is the first room then run the game_start event for the room
			engine->current_room->game_start();
		}
		engine->current_room->room_start(); // Run the room_start event for the new room

		if (!get_option("is_headless").i) {
			engine->current_room->draw(); // Run the draw event for the new room
		}

		if ((engine->transition_type != E_TRANSITION::NONE)&&(!get_option("is_headless").i)) { // If a transition has been defined then finish drawing the new room into the after buffer
			render::reset_target();
			draw_transition(); // Animate the defined transition from the before and after buffers
		}

		if (should_jump) { // If we should jump to the end of the event loop
			throw 0; // Throw an exception
		}

		return 0;
	}
	/**
	* Handle room transitions and resource changes between rooms.
	* @note If the function is called without the should_jump flag, then let it be true.
	* @param new_room the room to change to
	*
	* @retval 0 success
	*/
	int change_room(Room* new_room) {
		return change_room(new_room, true);
	}

	/**
	* @note While it is possible for this to return nullptr, in normal practice this should never happen because the event loop will refuse to run.
	* @returns the current Room resource
	*/
	Room* get_current_room() {
		return engine->current_room;
	}
	/**
	* @returns whether the event loop is currently processing events
	*/
	bool get_is_ready() {
		return engine->is_ready;
	}
	/**
	* @returns the size of the current room
	*/
	std::pair<int,int> get_room_size() {
		if (engine->current_room == nullptr) {
			return std::make_pair(-1, -1);
		}
		return std::make_pair(engine->current_room->get_width(), engine->current_room->get_height());
	}
	/**
	* @returns whether the given rectangle will appear on screen
	*/
	bool is_on_screen(const SDL_Rect& rect) {
		SDL_Rect screen = {0, 0, get_room_size().first, get_room_size().second}; // Initialize a rectangle for the window dimensions
		return util::check_collision(rect, screen); // Return whether the given rectangle collides with the screen's rectangle
	}

	/**
	* Set the pause state of the engine and return the previous state.
	* @note This will stop processing non-draw events for all objects which have is_pausable set to true.
	* @param is_paused whether to pause or not
	*
	* @returns the previous pause state
	*/
	bool set_is_paused(bool is_paused) {
		bool p = engine->is_paused;
		engine->is_paused = is_paused;
		return p;
	}
	/**
	* @returns the pause state of the engine
	*/
	bool get_is_paused() {
		return engine->is_paused;
	}
}

#endif // BEE_CORE_ROOMS
