/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_KEYBIND_H
#define BEE_CORE_KEYBIND_H 1

#include <string>

#include <SDL2/SDL.h>

namespace bee {
	struct KeyBind {
		SDL_Keycode key;
		std::string command;
		bool is_repeatable;

		KeyBind(SDL_Keycode, const std::string&, bool);
		KeyBind();
		explicit KeyBind(const std::string&);
		KeyBind(const std::string&, bool);
	};
}

#endif // BEE_CORE_KEYBIND_H
