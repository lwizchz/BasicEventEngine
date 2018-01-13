/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_KEYBIND_H
#define BEE_INPUT_KEYBIND_H 1

#include <string>
#include <functional>

#include <SDL2/SDL.h>

namespace bee {
	struct KeyBind {
		std::string name;
		SDL_Keycode key;
		std::function<void (const SDL_Event*)> func;
		bool is_repeatable;

		KeyBind(const std::string&, SDL_Keycode, bool, std::function<void (const SDL_Event*)>);
		KeyBind();
		KeyBind(const std::string&);
		KeyBind(const std::string&, std::function<void (const SDL_Event*)>);
		KeyBind(const std::string&, bool, std::function<void (const SDL_Event*)>);

		void call(const SDL_Event*);
	};
}

#endif // BEE_INPUT_KEYBIND_H
