/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_DISPLAY
#define _BEE_GAME_DISPLAY 1

#include <SDL2/SDL.h> // Include the necessary SDL functions

#include "../game.hpp" // Include the engine headers

/*
* BEE::get_display() - Return the SDL display mode, i.e. display format, width, height, refresh rate, and driver data
* ! See https://wiki.libsdl.org/SDL_DisplayMode for details
*/
SDL_DisplayMode BEE::get_display() const {
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0) { // Attempt to get the desktop display mode
		std::cerr << "Failed to get display mode: " << SDL_GetError() << "\n"; // Output error string
	}
	return dm; // Return the data, note that if the previous call failed then the structure will be empty
}
/*
* BEE::get_display_format() - Return the pixel format of the display
*/
Uint32 BEE::get_display_format() const {
	return get_display().format;
}
/*
* BEE::get_display_width() - Return the width of the display
*/
int BEE::get_display_width() const {
	return get_display().w;
}
/*
* BEE::get_display_height() - Return the height of the display
*/
int BEE::get_display_height() const {
	return get_display().h;
}
/*
* BEE::get_display_refresh_rate() - Return the refresh rate of the display
*/
int BEE::get_display_refresh_rate() const {
	return get_display().refresh_rate;
}

/*
* BEE::set_display() - Set the SDL display mode's width, height, and refresh rate
* @w: the new width to set the display to
* @h: the new height to set the display to
* @hz: the new refresh rate to set the display to
* ! See https://wiki.libsdl.org/SDL_SetWindowDisplayMode for details
*/
int BEE::set_display(int w, int h, int hz) const {
	if (options->is_fullscreen) { // Only set the display mode when the window is fullscreen
		SDL_DisplayMode dm = {get_display_format(), w, h, hz, 0}; // Define a new display mode using the current pixel format
		if (SDL_SetWindowDisplayMode(window, &dm) != 0) { // Attempt to set the display mode that should be used when the window is visible
			std::cerr << "Failed to set display mode: " << SDL_GetError() << "\n"; // Output error string
			return 1; // Return 1 on display mode fail
		}
		return 0; // Return 0 on successful display mode set
	} else {
		std::cerr << "Failed to set display size because the window is not fullscreen.\n"; // Output error string
		return 2; // Return 2 when the window is not fullscreen
	}
}
/*
* BEE::set_display_size() - Set the width and height of the display
* @w: the new width to set the display to
* @h: the new height to set the display to
*/
int BEE::set_display_size(int w, int h) const {
	return set_display(w, h, get_display_refresh_rate()); // Set the display size using the current refresh rate
}
/*
* BEE::set_display_refresh_rate() - Set the refresh rate of the display
* @hz: the new refresh rate to set the display to
*/
int BEE::set_display_refresh_rate(int hz) const {
	return set_display(get_display_width(), get_display_height(), hz); // Set the display refresh rate using the current size
}

#endif // _BEE_GAME_DISPLAY
