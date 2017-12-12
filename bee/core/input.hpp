/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_INPUT_H
#define BEE_CORE_INPUT_H 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	// Forward declaration
	class Instance;

	std::pair<int,int> get_mouse_global_position();
	int get_mouse_global_x();
	int get_mouse_global_y();
	std::pair<int,int> get_mouse_position();
	int get_mouse_x();
	int get_mouse_y();
	int set_mouse_global_position(int, int);
	int set_mouse_global_x(int);
	int set_mouse_global_y(int);

	bool is_mouse_inside(const Instance&);

	int set_mouse_is_visible(bool);

	bool get_key_state(SDL_Scancode);
	bool get_key_state(SDL_Keycode);
	bool get_mod_state(Uint8);
	bool get_mouse_state(Uint8);
	int get_mouse_wheel_flip(const SDL_MouseWheelEvent&);

	char append_input(std::string*, SDL_KeyboardEvent*);

	int keystrings_populate();
	SDL_Keycode keystrings_get_key(const std::string&);
	std::string keystrings_get_string(SDL_Keycode);
	std::string keystrings_get_name(SDL_Keycode);
}

#endif // BEE_CORE_INPUT_H
