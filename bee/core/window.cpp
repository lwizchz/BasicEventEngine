/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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

#include "../resource/texture.hpp"

namespace bee {
	/**
	* @see https://wiki.libsdl.org/SDL_GetWindowTitle for details
	* @returns the current window title string
	*/
	std::string get_window_title() {
		return SDL_GetWindowTitle(engine->renderer->window);
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
	* @note Give -1 for a dimension in order to leave it unchanged.
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

		engine->texture_before->free();
		engine->texture_after->free();
		engine->texture_before->load_as_target(engine->width, engine->height);
		engine->texture_after->load_as_target(engine->width, engine->height);
	}
}

#endif // BEE_CORE_WINDOW
