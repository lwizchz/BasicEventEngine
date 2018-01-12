/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_MOUSE
#define BEE_INPUT_MOUSE 1

#include <tuple>

#include "mouse.hpp"

#include "../util/collision.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/rooms.hpp"

#include "../render/renderer.hpp"
#include "../render/viewport.hpp"

#include "../resource/texture.hpp"
#include "../resource/object.hpp"
#include "../resource/room.hpp"

namespace bee { namespace mouse {
	namespace internal {
		SDL_Cursor* cursor = nullptr;
	}

	/**
	* Initialize the mouse cursor to the standard arrow.
	*/
	void init() {
		internal::cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		SDL_SetCursor(internal::cursor);
	}
	/**
	* Free the mouse cursor.
	*/
	void close() {
		SDL_FreeCursor(internal::cursor);
		internal::cursor = nullptr;
	}

	/**
	* @returns the mouse coordinates relative to the display.
	* @see https://wiki.libsdl.org/SDL_GetMouseState for details
	*/
	std::pair<int,int> get_display_pos() {
		int mx, my;
		SDL_GetGlobalMouseState(&mx, &my); // Fetch the mouse coordinates into (mx, my)
		return std::make_pair(mx, my);
	}
	/**
	* @returns the mouse coordinates relative to the window.
	* @see https://wiki.libsdl.org/SDL_GetMouseState for details
	*/
	std::pair<int,int> get_pos() {
		int mx, my;
		SDL_GetMouseState(&mx, &my); // Fetch the mouse coordinates into (mx, my)
		return std::make_pair(mx, my);
	}
	/**
	* @returns the mouse coordinates relative to the viewport
	*/
	std::pair<int,int> get_relative_pos() {
		int mx, my;
		std::tie(mx, my) = get_pos(); // Fetch the global coordinates into (mx, my)

		ViewPort* v = get_current_room()->get_current_view(); // Get the current view
		if ((v == nullptr)&&(get_current_room()->get_views().size() > 0)) { // If this function is called outside of view drawing then simply use the first view
			v = get_current_room()->get_views().front();
		}

		if (v != nullptr) { // If the view exists then adjust the coordinates
			mx -= v->port.x;
			my -= v->port.y;
		}

		return std::make_pair(mx, my); // Return the pair on success
	}
	/**
	* Set the mouse coordinates relative to the display.
	* @see https://wiki.libsdl.org/SDL_WarpMouseGlobal for details
	* @param new_mx the x-coordinate to move the mouse to
	* @param new_my the y-coordinate to move the mouse to
	*
	* @retval 0 success
	* @retval -1 failure
	*/
	int set_display_pos(int new_mx, int new_my) {
		#if SDL_VERSION_ATLEAST(2, 0, 4)
			return SDL_WarpMouseGlobal(new_mx, new_my); // Move the mouse to the given coordinates
		#endif

		return -1;
	}
	/**
	* Set the mouse coordinates relative to the window.
	* @see https://wiki.libsdl.org/SDL_WarpMouseInWindow for details
	* @param new_mx the x-coordinate to move the mouse to
	* @param new_my the y-coordinate to move the mouse to
	*/
	void set_pos(int new_mx, int new_my) {
		SDL_WarpMouseInWindow(engine->renderer->window, new_mx, new_my); // Move the mouse to the given coordinates
	}

	/**
	* @param instance the instance to check a collision for
	* @returns whether the mouse collides with the instance's bounding box
	*/
	bool is_inside(const Instance* instance) {
		Texture* m = instance->get_object()->get_mask();
		if (m == nullptr) {
			return false;
		}

		SDL_Rect inst = {static_cast<int>(instance->get_x()), static_cast<int>(instance->get_y()), m->get_width(), m->get_height()}; // Create a bounding box based on the instance's mask
		SDL_Rect mouse = {get_pos().first, get_pos().second, 1, 1};
		return check_collision(inst, mouse); // Return whether the instance collides with the mouse
	}

	/**
	* @see https://wiki.libsdl.org/SDL_GetMouseState for details
	* @param b the given button to check
	* @returns whether the given mouse button is pressed
	*/
	bool get_state(Uint8 b) {
		return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(b)); // Return the button state of the given button as a boolean value
	}
	/**
	* @see https://wiki.libsdl.org/SDL_MouseWheelEvent for details
	* @param wheel the mouse event to check
	* @returns a scalar for mouse wheel flipping
	*/
	int get_wheel_flip(const SDL_MouseWheelEvent& wheel) {
		#if SDL_VERSION_ATLEAST(2, 0, 4)
			if (wheel.direction == SDL_MOUSEWHEEL_FLIPPED) {
				return -1;
			}
		#endif

		return 1;
	}


	/**
	* @returns the current window cursor
	*/
	SDL_Cursor* get_cursor()  {
		return internal::cursor;
	}
	/**
	* Change the window cursor to the given type.
	* @see https://wiki.libsdl.org/SDL_CreateSystemCursor for details
	* @param cid the SDL system cursor enum id
	*
	* @retval 0 success
	* @retval 1 failed to create cursor
	*/
	int set_cursor(SDL_SystemCursor cid) {
		SDL_Cursor* cursor = SDL_CreateSystemCursor(cid);
		if (cursor == nullptr) {
			messenger::send({"engine", "mouse"}, E_MESSAGE::WARNING, std::string("Failed to create system cursor ") + std::to_string(static_cast<int>(cid)) + ": " + SDL_GetError());
			return 1;
		}

		SDL_FreeCursor(internal::cursor);
		internal::cursor = cursor;
		SDL_SetCursor(internal::cursor);

		return 0;
	}
	/**
	* Change the window cursor to the given texture.
	* @note The image will be reloaded and stored separately from the given Texture resource.
	* @see https://wiki.libsdl.org/SDL_CreateColorCursor for details
	*
	* @param tex the texture to load
	* @param x the x-coordinate of the cursor's hotspot
	* @param y the y-coordinate of the cursor's hotspot
	*
	* @retval 0 success
	* @retval 1 failed to load image surface
	* @retval 2 failed to create cursor
	*/
	int set_cursor(const Texture* tex, int x, int y) {
		SDL_Surface* surface = tex->load_surface();
		if (surface == nullptr) {
			return 1;
		}

		SDL_Cursor* cursor = SDL_CreateColorCursor(surface, x, y);
		if (cursor == nullptr) {
			messenger::send({"engine", "mouse"}, E_MESSAGE::WARNING, "Failed to create cursor from surface \"" + tex->get_path() + "\": " + SDL_GetError());
			return 2;
		}

		SDL_FreeCursor(internal::cursor);
		internal::cursor = cursor;
		SDL_SetCursor(internal::cursor);

		return 0;
	}
	/**
	* Set whether to show the cursor or not.
	* @param show_cursor whether the cursor should be visible or not
	*
	* @retval 0 success
	* @retval 1 failed to change the cursor visibility
	*/
	int set_show_cursor(bool show_cursor) {
		if (SDL_ShowCursor((show_cursor) ? SDL_ENABLE : SDL_DISABLE) < 0) {
			return 1;
		}
		return 0;
	}
}}

#endif // BEE_INPUT_MOUSE
