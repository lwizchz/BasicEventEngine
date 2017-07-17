/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI_H
#define BEE_UI_H

#include <string> // Include required library headers
#include <functional>

#include "../render/rgba.hpp"

namespace bee {
	// Forward declarations
	class Instance;
	class Sound;
	class Font;

namespace ui {
	namespace internal {
		// Declare sounds
		extern Sound* snd_button_press;
		extern Sound* snd_button_release;
	}

	int load();
	int free();

	int destroy_parent(Instance*);

	Instance* create_button(int, int, Font*, const std::string&, RGBA, std::function<void (Instance*)>);
	int button_callback(Instance*);

	Instance* create_handle(int, int, int, int, RGBA, Instance*);
	int destroy_handle(Instance*);
}}

#endif // BEE_UI_H
