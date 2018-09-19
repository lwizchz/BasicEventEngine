/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_KB_H
#define BEE_INPUT_KB_H 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee {
	// Forward declaration
	class Instance;
	struct KeyBind;
namespace kb {
	namespace internal {
		void handle_input(const SDL_Event*);
	}

	void init();

	bool get_state(SDL_Scancode);
	bool get_state(SDL_Keycode);
	SDL_Keymod get_mod_state();
	bool get_mod_state(int);

	char append_input(std::string*, const SDL_KeyboardEvent*);

	SDL_Keycode keystrings_get_key(const std::string&);
	std::string keystrings_get_string(SDL_Keycode);
	std::string keystrings_get_name(SDL_Keycode);

	int bind(SDL_Keycode, KeyBind);
	KeyBind get_keybind(SDL_Keycode);
	KeyBind get_keybind(const std::string&);
	int unbind(SDL_Keycode);
	int unbind(KeyBind);
	void unbind_all();
}}

#endif // BEE_INPUT_KB_H
