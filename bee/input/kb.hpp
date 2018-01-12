/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_KB_H
#define BEE_INPUT_KB_H 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	// Forward declaration
	class Instance;
namespace kb {
	void init();

	bool get_state(SDL_Scancode);
	bool get_state(SDL_Keycode);
	SDL_Keymod get_mod_state();
	bool get_mod_state(int);

	char append_input(std::string*, SDL_KeyboardEvent*);

	SDL_Keycode keystrings_get_key(const std::string&);
	std::string keystrings_get_string(SDL_Keycode);
	std::string keystrings_get_name(SDL_Keycode);
}}

#endif // BEE_INPUT_KB_H
