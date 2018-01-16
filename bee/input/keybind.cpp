/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INPUT_KEYBIND
#define BEE_INPUT_KEYBIND 1

#include "keybind.hpp"

namespace bee {
	KeyBind::KeyBind(const std::string& _name, SDL_Keycode _key, bool _is_repeatable, std::function<void (const SDL_Event*)> _func) :
		name(_name),
		key(_key),
		func(_func),
		is_repeatable(_is_repeatable)
	{}
	KeyBind::KeyBind() :
		KeyBind("", SDLK_UNKNOWN, false, nullptr)
	{}
	KeyBind::KeyBind(const std::string& _name) :
		KeyBind(_name, SDLK_UNKNOWN, false, nullptr)
	{}
	KeyBind::KeyBind(const std::string& _name, std::function<void (const SDL_Event*)> _func) :
		KeyBind(_name, SDLK_UNKNOWN, false, _func)
	{}
	KeyBind::KeyBind(const std::string& _name, bool _is_repeatable, std::function<void (const SDL_Event*)> _func) :
		KeyBind(_name, SDLK_UNKNOWN, _is_repeatable, _func)
	{}

	void KeyBind::call(const SDL_Event* e) {
		if (func != nullptr) {
			func(e);
		}
	}
}

#endif // BEE_INPUT_KEYBIND
