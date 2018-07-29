/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_DISPLAY
#define BEE_CORE_DISPLAY 1

#include <SDL2/SDL.h> // Include the required SDL headers

#include "display.hpp"

#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "enginestate.hpp"

#include "../render/renderer.hpp"

namespace bee {
	/**
	* Return the SDL display mode: display format, width, height, refresh rate, and driver data.
	* @see https://wiki.libsdl.org/SDL_DisplayMode for details
	* @returns the current SDL_DisplayMode
	*/
	SDL_DisplayMode get_display() {
		SDL_DisplayMode dm;
		if (SDL_GetDesktopDisplayMode(0, &dm) != 0) { // Attempt to get the desktop display mode
			messenger::send({"engine"}, E_MESSAGE::WARNING, "Failed to get display mode: " + util::get_sdl_error());
		}
		return dm; // Return the data, note that if the previous call failed then the structure will be empty
	}
	/**
	* @returns the display dimensions
	*/
	std::pair<int,int> get_display_size() {
		SDL_DisplayMode dm (get_display());
		return std::make_pair(dm.w, dm.h);
	}

	/**
	* Set the SDL display mode's width, height, and refresh rate.
	* @see https://wiki.libsdl.org/SDL_SetWindowDisplayMode for details
	* @param w the new width to set the display to
	* @param h the new height to set the display to
	* @param hz the new refresh rate to set the display to
	*
	* @retval 0 success
	* @retval 1 failed to set display mode
	* @retval 2 not in fullscreen mode, so the display mode is not relevant
	*/
	int set_display(int w, int h, int hz) {
		if (get_option("is_fullscreen").i) { // Only set the display mode when the window is fullscreen
			SDL_DisplayMode dm = {get_display().format, w, h, hz, 0}; // Define a new display mode using the current pixel format
			if (SDL_SetWindowDisplayMode(engine->renderer->window, &dm) != 0) {
				messenger::send({"engine"}, E_MESSAGE::WARNING, "Failed to set display mode: " + util::get_sdl_error());
				return 1;
			}
			return 0;
		} else {
			messenger::send({"engine"}, E_MESSAGE::WARNING, "Failed to set display size because the window is not fullscreen.");
			return 2;
		}
	}
	/**
	* Set the width and height of the display
	* @param w the new width to set the display to
	* @param h the new height to set the display to
	*/
	int set_display_size(int w, int h) {
		return set_display(w, h, get_display().refresh_rate); // Set the display size using the current refresh rate
	}
	/**
	* Set the refresh rate of the display
	* @param hz the new refresh rate to set the display to
	*/
	int set_display_refresh_rate(int hz) {
		SDL_DisplayMode dm (get_display());
		return set_display(dm.w, dm.h, hz); // Set the display refresh rate using the current size
	}
}

#endif // BEE_CORE_DISPLAY
