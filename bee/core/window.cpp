/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_WINDOW
#define BEE_CORE_WINDOW 1

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

#include "window.hpp"

#include "enginestate.hpp"

#include "../render/renderer.hpp"

namespace bee {
	/**
	* @see https://wiki.libsdl.org/SDL_GetWindowTitle for details
	* @returns the current window title string
	*/
	std::string get_window_title() {
		return SDL_GetWindowTitle(engine->renderer->window);
	}
	/**
	* @returns the current window cursor
	*/
	SDL_Cursor* get_cursor()  {
		return engine->cursor;
	}
	/**
	* @returns the window coordinates and dimensions
	*/
	SDL_Rect get_window() {
		std::pair<int,int> pos (get_window_pos());
		std::pair<int,int> size (get_window_size());
		return SDL_Rect({pos.first, pos.second, size.first, size.second});
	}
	/**
	* @returns the position of the game window
	*/
	std::pair<int,int> get_window_pos() {
		int wx, wy;
		SDL_GetWindowPosition(engine->renderer->window, &wx, &wy);
		return std::make_pair(wx, wy);
	}
	/**
	* @returns the size of the game window
	*/
	std::pair<int,int> get_window_size() {
		return std::make_pair(engine->width, engine->height);
	}

	/**
	* Set the title string of the current window.
	* @param title the string to set the title to
	*/
	void set_window_title(const std::string& title) {
		SDL_SetWindowTitle(engine->renderer->window, title.c_str());
	}
	/**
	* Change the current window cursor to the given type.
	* @see https://wiki.libsdl.org/SDL_CreateSystemCursor for details
	* @param cid the SDL system cursor enum id
	*
	* @retval 0 success
	* @retval 1 failed to create cursor
	*/
	int set_cursor(SDL_SystemCursor cid) {
		SDL_Cursor* cursor = SDL_CreateSystemCursor(cid);
		if (cursor == nullptr) {
			return 1;
		}

		SDL_FreeCursor(engine->cursor);
		engine->cursor = cursor;
		SDL_SetCursor(engine->cursor);

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
	/**
	* Set the game window position.
	* @note Give -1 for a coordinate in order to leave it unchanged.
	* @param x the new x-coordinate to move the window to
	* @param y the new y-coordinate to move the window to
	*/
	void set_window_position(int x, int y) {
		if (x < 0) {
			x = get_window_pos().first;
		}
		if (y < 0) {
			y = get_window_pos().second;
		}

		SDL_SetWindowPosition(engine->renderer->window, x, y);
	}
	/**
	* Center the game window on the screen.
	*/
	void set_window_center() {
		set_window_position(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
	/**
	* Set the size of the game window.
	* @note Give -1 for a dimension in order to leave it unchanged
	* @param width the new width to change the window to
	* @param height the new height to change the window to
	*/
	void set_window_size(int width, int height) {
		if (width < 0) {
			width = engine->width;
		}
		if (height < 0) {
			height = engine->height;
		}

		engine->width = width;
		engine->height = height;
		SDL_SetWindowSize(engine->renderer->window, engine->width, engine->height);
	}
}

#endif // BEE_CORE_WINDOW
