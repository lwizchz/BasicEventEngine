/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
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
#include "resources.hpp"
#include "window.hpp"

#include "../render/camera.hpp"
#include "../render/render.hpp"
#include "../render/renderer.hpp"
#include "../render/shader.hpp"
#include "../render/transition.hpp"
#include "../render/viewdata.hpp"

#include "../resource/room.hpp"

namespace bee {
	/*
	* restart_room() - Throw an exception in order to jump to the end of the event loop
	*/
	void restart_room() {
		throw 3; // Throw the exception
	}
	/*
	* change_room() - Handle room transitions and resource changes between rooms
	* ! Currently transitions are drawn even if none is set because without it nothing will draw for some reason
	* @new_room: the room to change to
	* @should_jump: whether we should jump to the end of the event loop or not when we finish setting up the new room
	*/
	int change_room(Room* new_room, bool should_jump) {
		if (((engine->quit)^(new_room != nullptr)) == false) { // Abort the room change if either the quit flag is set without a null room or if the quit flag is unset with a null room
			throw 0; // Throw an exception to jump to the end of the event loop
			return 1; // Return 1 to satisfy the compiler
		}

		bool is_game_start = false;
		if (engine->current_room != nullptr) { // if we are currently in a room
			if (engine->transition_type != E_TRANSITION::NONE) { // If a transition has been defined then draw the current room into the before buffer
				set_render_target(engine->texture_before);
				engine->renderer->render_clear();
				engine->current_room->draw();
				engine->renderer->render();
			}
			if (new_room != nullptr) {
				engine->current_room->room_end(); // Run the room_end event for the current room
				engine->current_room->reset_properties(); // Reset the current room's properties
			}
		} else { // if we are not in a room
			if (engine->transition_type != E_TRANSITION::NONE) { // If a transition has been defined then draw nothing into the before buffer
				set_render_target(engine->texture_before);
				engine->renderer->render_clear();
				engine->renderer->render();
			}
			is_game_start = true;
			engine->first_room = new_room; // Set the new room as the first room
		}

		sound_stop_loops(); // Stop all looping sounds from the previous room

		if (new_room == nullptr) { // If we're transitioning to a null room, i.e. the game is ending
			if (engine->transition_type != E_TRANSITION::NONE) { // If a transition has been defined then prepare for drawing an empty room into the after buffer
				set_render_target(engine->texture_after);
				engine->renderer->render_clear();
				engine->renderer->render();
				reset_render_target();
				draw_transition(); // Animate the defined transition from the before and after buffers
			}
			engine->current_room->reset_properties(); // Reset the current room's properties
			engine->current_room = nullptr;
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
			set_render_target(engine->texture_after);
		} else { // Otherwise reset the render target just to be sure
			reset_render_target();
		}
		if (!get_options().is_headless) {
			engine->renderer->render_clear();
		}

		engine->is_ready = true; // Set the event loop as running
		messenger::handle();
		engine->current_room->create(); // Run the create event for the new room
		if (is_game_start) { // If this is the first room then run the game_start event for the room
			engine->current_room->game_start();
		}
		engine->current_room->room_start(); // Run the room_start event for the new room

		if (!get_options().is_headless) {
			engine->current_room->draw(); // Run the draw event for the new room
		}

		if ((engine->transition_type != E_TRANSITION::NONE)&&(!get_options().is_headless)) { // If a transition has been defined then finish drawing the new room into the after buffer
			engine->renderer->render();
			reset_render_target();
			draw_transition(); // Animate the defined transition from the before and after buffers
		}

		if (should_jump) { // If we should jump to the end of the event loop
			throw 0; // Throw an exception
		}

		return 0; // Return 0 on success
	}
	/*
	* change_room() - Handle room transitions and resource changes between rooms
	* ! When the function is called without the should_jump flag, then simply call the function again with it set to true
	* @new_room: the room to change to
	*/
	int change_room(Room* new_room) {
		return change_room(new_room, true);
	}
	/*
	* room_goto() - Change to the room with the given id
	* @id: the id of the room to change to
	*/
	int room_goto(int id) {
		if (Room::get(id) != nullptr) { // If the room exists, change to it
			return change_room(Room::get(id));
		}
		return 3; // Return 3 on non-existent room
	}
	/*
	* room_goto_previous() - Change to the room which was added to the resource list before the current room
	*/
	int room_goto_previous() {
		return room_goto(get_current_room()->get_id()-1);
	}
	/*
	* room_goto_next() - Change to the room which was added to the resource list after the current room
	*/
	int room_goto_next() {
		return room_goto(get_current_room()->get_id()+1);
	}

	/*
	* get_current_room() - Return the current room resource
	* ! While it is possible for this to return nullptr, in practice this should never happen because the event loop will refuse to run
	*/
	Room* get_current_room() {
		return engine->current_room;
	}
	/*
	* get_is_ready() - Return whether the event loop is currently processing events
	*/
	bool get_is_ready() {
		return engine->is_ready;
	}
	/*
	* get_room_width() - Return the width of the current room
	*/
	int get_room_width() {
		if (engine->current_room != nullptr) {
			return engine->current_room->get_width();
		}
		return -1;
	}
	/*
	* get_room_height() - Return the height of the current room
	*/
	int get_room_height() {
		if (engine->current_room != nullptr) {
			return engine->current_room->get_height();
		}
		return -1;
	}
	/*
	* is_on_screen() - Return whether the given rectangle will appear on screen
	*/
	bool is_on_screen(const SDL_Rect& rect) {
		SDL_Rect screen = {0, 0, get_room_width(), get_room_height()}; // Initialize a rectangle for the window dimensions
		return check_collision(rect, screen); // Return whether the given rectangle collides with the screen's rectangle
	}

	/*
	* set_viewport() - Set the new drawing viewport within the window
	* ! See https://wiki.libsdl.org/SDL_RenderSetViewport for details
	* @viewport: the rectangle defining the desired viewport
	*/
	int set_viewport(ViewData* viewport) {
		if (get_options().renderer_type != E_RENDERER::SDL) {
			glm::mat4 view, projection;
			glm::vec4 port;

			if (viewport == nullptr) { // If the viewport is not defined then set the drawing area to the entire screen
				view = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
				port = glm::vec4(0.0f, 0.0f, get_room_width(), get_room_height());
				projection = render::get_projection();
			} else { // If the viewport is defined then use it
				view = glm::translate(glm::mat4(1.0f), glm::vec3(viewport->view.x, viewport->view.y, 0.0f));
				port = glm::vec4(viewport->port.x, viewport->port.y, viewport->port.w, viewport->port.h);
				render::set_camera(new Camera(
					static_cast<float>(viewport->view.w),
					static_cast<float>(viewport->view.h)
				));
				projection = render::get_projection();
			}

			glUniformMatrix4fv(engine->renderer->program->get_location("view"), 1, GL_FALSE, glm::value_ptr(view));
			glUniform4fv(engine->renderer->program->get_location("port"), 1, glm::value_ptr(port));
			glUniformMatrix4fv(engine->renderer->program->get_location("projection"), 1, GL_FALSE, glm::value_ptr(projection));

			return 0;
		} else {
			SDL_Rect v;
			if (viewport == nullptr) { // If the viewport is not defined then set the drawing area to the entire screen
				v = {0, 0, get_width(), get_height()};
			} else { // If the viewport is defined then use it
				v = viewport->port;
			}
			return SDL_RenderSetViewport(engine->renderer->sdl_renderer, &v);
		}
	}

	/*
	* set_is_paused() - Set the pause state of the engine and return the previous state
	* ! Note that this will stop processing non-draw events for all objects which have is_pausable set to true
	*/
	bool set_is_paused(bool is_paused) {
		bool p = engine->is_paused;
		engine->is_paused = is_paused;
		return p;
	}
	/*
	* get_is_paused() - Return the pause state of the engine
	*/
	bool get_is_paused() {
		return engine->is_paused;
	}
}

#endif // BEE_CORE_ROOMS
