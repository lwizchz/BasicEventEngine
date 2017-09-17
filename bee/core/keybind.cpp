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
	KeyBind::KeyBind(SDL_Keycode new_key, const std::string& new_command, bool new_is_repeatable) :
		key(new_key),
		command(new_command),
		is_repeatable(new_is_repeatable)
	{}
	KeyBind::KeyBind() :
		KeyBind(SDLK_UNKNOWN, "", false)
	{}
	KeyBind::KeyBind(const std::string& new_command) :
		KeyBind(SDLK_UNKNOWN, new_command, false)
	{}
	KeyBind::KeyBind(const std::string& new_command, bool new_is_repeatable) :
		KeyBind(SDLK_UNKNOWN, new_command, new_is_repeatable)
	{}
}

#endif // BEE_CORE_KEYBIND
