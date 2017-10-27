/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_KEYBIND
#define BEE_CORE_KEYBIND 1

#include "keybind.hpp" // Include the engine headers

#include "../util/string.hpp"

namespace bee {
	KeyBind::KeyBind(SDL_Keycode _key, const std::string& _command, bool _is_repeatable) :
		key(_key),
		command(_command),
		is_repeatable(_is_repeatable)
	{}
	KeyBind::KeyBind() :
		KeyBind(SDLK_UNKNOWN, "", false)
	{}
	KeyBind::KeyBind(const std::string& _command) :
		KeyBind(SDLK_UNKNOWN, _command, false)
	{}
	KeyBind::KeyBind(const std::string& _command, bool _is_repeatable) :
		KeyBind(SDLK_UNKNOWN, _command, _is_repeatable)
	{}
}

#endif // BEE_CORE_KEYBIND
