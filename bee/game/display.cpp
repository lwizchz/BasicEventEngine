/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_DISPLAY
#define _BEE_GAME_DISPLAY 1

SDL_DisplayMode BEE::get_display() {
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0) {
		std::cerr << "Failed to get display mode: " << SDL_GetError() << "\n";
	}
	return dm;
}
Uint32 BEE::get_display_format() {
	return get_display().format;
}
int BEE::get_display_width() {
	return get_display().w;
}
int BEE::get_display_height() {
	return get_display().h;
}
int BEE::get_display_refresh_rate() {
	return get_display().refresh_rate;
}

int BEE::set_display(int w, int h, int hz) {
	if (options->is_fullscreen) {
		SDL_DisplayMode dm = {get_display_format(), w, h, hz, 0};
		if (SDL_SetWindowDisplayMode(window, &dm) != 0) {
			std::cerr << "Failed to set display size: " << SDL_GetError() << "\n";
			return 1;
		}
		return 0;
	} else {
		std::cerr << "Failed to set display size because the window is not fullscreen.\n";
		return 1;
	}
}
int BEE::set_display_size(int w, int h) {
	return set_display(w, h, get_display_refresh_rate());
}
int BEE::set_display_refresh_rate(int hz) {
	return set_display(get_display_width(), get_display_height(), hz);
}

#endif // _BEE_GAME_DISPLAY
