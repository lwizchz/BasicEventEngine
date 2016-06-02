/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_ROOM
#define _BEE_GAME_ROOM 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::restart_room() - Throw an exception in order to jump to the end of the event loop
*/
void BEE::restart_room() const {
	throw 3; // Throw the exception
}
/*
* BEE::change_room() - Handle room transitions and resource changes between rooms
* ! Currently transitions are drawn even if none is set because without it nothing will draw for some reason
* @new_room: the room to change to
* @should_jump: whether we should jump to the end of the event loop or not when we finish setting up the new room
*/
int BEE::change_room(Room* new_room, bool should_jump) {
	if (quit) { // If the quit flag is set then abort room change
		throw 4; // Throw an exception to jump to the end of the event loop
		return 1; // Return 1 to satisfy the compiler
	}

	if (new_room == nullptr) { // If the given room is nullptr then abort
		return 1; // Return 1 on invalid room resource
	}

	bool is_game_start = false;
	if (current_room != nullptr) { // if we are currently in a room
		if (transition_type != 0) { // If a transition has been defined then draw the current room into the before buffer
			set_render_target(texture_before);
			render_clear();
			current_room->draw();
			render();
		}
		current_room->room_end(); // Run the room_end event for the current room
		current_room->reset_properties(); // Reset the current room's properties
	} else { // if we are not in a room
		if (transition_type != 0) { // If a transition has been defined then draw nothing into the before buffer
			set_render_target(texture_before);
			render_clear();
			render();
		}
		is_game_start = true;
		first_room = new_room; // Set the new room as the first room
	}

	sound_stop_all(); // Stop all sounds from the previous room
	free_media(); // Free all resources from the previous room

	current_room = new_room; // Set the new room as the current room
	is_ready = false; // Set the event loop as not running
	current_room->reset_properties(); // Reset the new room's properties
	current_room->init(); // Initialize the room

	if (load_media()) { // Attempt to load all resources for the new room
		std::cerr << "Couldn't load room media for " << current_room->get_name() << "\n"; // Output error string
		return 2; // Return 2 on resource loading error
	}

	SDL_SetWindowTitle(window, current_room->get_name().c_str()); // Set the window title to the room's name
	std::cout << current_room->get_instance_string(); // Output the list of instances in the room

	if (transition_type != 0) { // If a transition has been defined then prepare for drawing the new room into the after buffer
		set_render_target(texture_after);
	} else { // Otherwise reset the render target just to be sure
		reset_render_target();
	}
	render_clear();

	is_ready = true; // Set the event loop as running
	current_room->create(); // Run the create event for the new room
	if (is_game_start) { // If this is the first room then run the game_start event for the room
		current_room->game_start();
	}
	current_room->room_start(); // Run the room_start event for the new room
	current_room->draw(); // Run the draw event for the new room

	if (transition_type != 0) { // If a transition has been defined then finish drawing the new room into the after buffer
		render();
		reset_render_target();
		draw_transition(); // Animate the defined transition from the before and after buffers
	}

	if (should_jump) { // If we should jump to the end of the event loop
		throw 4; // Throw an exception
	}

	return 0; // Return 0 on success
}
/*
* BEE::change_room() - Handle room transitions and resource changes between rooms
* ! When the function is called without the should_jump flag, then simply call the function again with it set to true
* @new_room: the room to change to
*/
int BEE::change_room(Room* new_room) {
	return change_room(new_room, true);
}
/*
* BEE::room_goto() - Change to the room with the given id
* @id: the id of the room to change to
*/
int BEE::room_goto(int id) {
	if (get_room(id) != nullptr) { // If the room exists, change to it
		return change_room(get_room(id));
	}
	return 3; // Return 3 on non-existent room
}
/*
* BEE::room_goto_previous() - Change to the room which was added to the resource list before the current room
*/
int BEE::room_goto_previous() {
	return room_goto(get_current_room()->get_id()-1);
}
/*
* BEE::room_goto_next() - Change to the room which was added to the resource list after the current room
*/
int BEE::room_goto_next() {
	return room_goto(get_current_room()->get_id()+1);
}

/*
* BEE::get_current_room() - Return the current room resource
* ! While it is possible for this to return nullptr, in practice this should never happen because the event loop will refuse to run
*/
BEE::Room* BEE::get_current_room() const {
	return current_room;
}
/*
* BEE::get_is_ready() - Return whether the event loop is currently processing events
*/
bool BEE::get_is_ready() const {
	return is_ready;
}
/*
* BEE::get_room_width() - Return the width of the current room
*/
int BEE::get_room_width() const {
	if (current_room != nullptr) {
		return current_room->get_width();
	}
	return -1;
}
/*
* BEE::get_room_height() - Return the height of the current room
*/
int BEE::get_room_height() const {
	if (current_room != nullptr) {
		return current_room->get_height();
	}
	return -1;
}
/*
* BEE::is_on_screen() - Return whether the given rectangle will appear on screen
*/
bool BEE::is_on_screen(const SDL_Rect& rect) const {
	SDL_Rect screen = {0, 0, get_room_width(), get_room_height()}; // Initialize a rectangle for the window dimensions
	return check_collision(rect, screen); // Return whether the given rectangle collides with the screen's rectangle
}

/*
* BEE::set_viewport() - Set the new drawing viewport within the window
* ! See https://wiki.libsdl.org/SDL_RenderSetViewport for details
* @viewport: the rectangle defining the desired viewport
*/
int BEE::set_viewport(ViewData* viewport) const {
	if (options->renderer_type != BEE_RENDERER_SDL) {
		glm::mat4 view, projection;
		glm::vec4 port;

		if (viewport == nullptr) { // If the viewport is not defined then set the drawing area to the entire screen
			view = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
			port = glm::vec4(0.0f, 0.0f, (float)get_room_width(), (float)get_room_height());
			projection = glm::ortho(0.0f, (float)get_width(), (float)get_height(), 0.0f, 0.0f, 10.0f);
		} else { // If the viewport is defined then use it
			view = glm::translate(glm::mat4(1.0f), glm::vec3((float)viewport->view_x, (float)viewport->view_y, 0.0f));
			port = glm::vec4(viewport->port_x, viewport->port_y, viewport->port_width, viewport->port_height);
			projection = glm::ortho(0.0f, (float)viewport->view_width, (float)viewport->view_height, 0.0f, 0.0f, 10.0f);
		}

		glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
		glUniform4fv(port_location, 1, glm::value_ptr(port));
		glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));

		return 0;
	} else {
		SDL_Rect v;
		if (viewport == nullptr) { // If the viewport is not defined then set the drawing area to the entire screen
			v = {0, 0, get_width(), get_height()};
		} else { // If the viewport is defined then use it
			v = {viewport->port_x, viewport->port_y, viewport->port_width, viewport->port_height};
		}
		return SDL_RenderSetViewport(renderer, &v);
	}
}

/*
* BEE::set_is_paused() - Set the pause state of the engine and return the previous state
* ! Note that this does not pause the event loop, it is simply meant as a way to globally check an arbitrary state
*/
bool BEE::set_is_paused(bool new_is_paused) {
	bool p = is_paused;
	is_paused = new_is_paused;
	return p;
}
/*
* BEE::get_is_paused() - Return the pause state of the engine
*/
bool BEE::get_is_paused() const {
	return is_paused;
}

#endif // _BEE_GAME_ROOM
