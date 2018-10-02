/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_KEYSTRINGS_H
#define BEE_INPUT_KEYSTRINGS_H 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

namespace bee { namespace kb {
	SDL_Keycode keystrings_get_key(const std::string&);
	std::string keystrings_get_string(SDL_Keycode);
}}

#endif // BEE_INPUT_KEYSTRINGS_H
