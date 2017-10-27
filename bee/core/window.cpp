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
	/*
	* get_window_title() - Return the current window title string
	* ! See https://wiki.libsdl.org/SDL_GetWindowTitle for details
	*/
	std::string get_window_title() {
		return SDL_GetWindowTitle(engine->renderer->window);
	}
	/*
	* get_cursor() - Return the current window cursor
	*/
	SDL_Cursor* get_cursor()  {
		return engine->cursor;
	}
	/*
	* get_window_x() - Return the x-coordinate of the game window
	*/
	int get_window_x() {
		int wx;
		SDL_GetWindowPosition(engine->renderer->window, &wx, nullptr);
		return wx;
	}
	/*
	* get_window_y() - Return the y-coordinate of the game window
	*/
	int get_window_y() {
		int wy;
		SDL_GetWindowPosition(engine->renderer->window, nullptr, &wy);
		return wy;
	}
	/*
	* get_width() - Return the width of the game window
	*/
	int get_width() {
		return engine->width;
	}
	/*
	* get_height() - Return the height of the game window
	*/
	int get_height() {
		return engine->height;
	}

	/*
	* set_window_title() - Set the title string of the current window
	* @title: the string to set the title to
	*/
	int set_window_title(const std::string& title) {
		SDL_SetWindowTitle(engine->renderer->window, title.c_str());
		return 0;
	}
	/*
	* set_cursor() - Change the current window cursor to the given type
	* ! See https://wiki.libsdl.org/SDL_CreateSystemCursor for details
	* @cid: the SDL system cursor enum id
	*/
	int set_cursor(SDL_SystemCursor cid) {
		SDL_FreeCursor(engine->cursor);
		engine->cursor = SDL_CreateSystemCursor(cid);
		SDL_SetCursor(engine->cursor);
		return 0;
	}
	/*
	* set_show_cursor() - Set whether to show the cursor or not
	* @show_cursor: whether the cursor should be visible or not
	*/
	int set_show_cursor(bool show_cursor) {
		SDL_ShowCursor((show_cursor) ? SDL_ENABLE : SDL_DISABLE);
		return 0;
	}
	/*
	* set_window_position() - Set the game window position
	* @x: the new x-coordinate to move the window to
	* @y: the new y-coordinate to move the window to
	*/
	int set_window_position(int x, int y) {
		SDL_SetWindowPosition(engine->renderer->window, x, y);
		return 0;
	}
	/*
	* set_window_x() - Set the x-coordinate of the game window
	* @x: the new x-coordinate to move the window to
	*/
	int set_window_x(int x) {
		return set_window_position(x, get_window_y());
	}
	/*
	* set_window_y() - Set the y-coordinate of the game window
	*/
	int set_window_y(int y) {
		return set_window_position(get_window_x(), y);
	}
	/*
	* set_window_center() - Center the game window on the screen
	*/
	int set_window_center() {
		return set_window_position(SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
	/*
	* set_window_size() - Change the size of the game window
	* @width: the new width to change the window to
	* @height: the new height to change the window to
	*/
	int set_window_size(int width, int height) {
		engine->width = width;
		engine->height = height;
		SDL_SetWindowSize(engine->renderer->window, engine->width, engine->height);
		return 0;
	}
	/*
	* set_width() - Change the width of the game window
	* @width: the new width to change the window to
	*/
	int set_width(int width) {
		return set_window_size(width, engine->height);
	}
	/*
	* set_height() - Change the height of the game window
	* @height: the new height to change the window to
	*/
	int set_height(int height) {
		return set_window_size(engine->width, height);
	}
}

#endif // BEE_CORE_WINDOW
