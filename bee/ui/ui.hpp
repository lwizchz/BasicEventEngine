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

#include "../core/sidp.hpp"

#include "../render/rgba.hpp"

namespace bee {
	// Forward declarations
	class Instance;
	class Sprite;
	class Font;
	class Object;

namespace ui {
	namespace internal {
		// Declare sprites
		extern Sprite* spr_button;

		// Declare objects
		extern Object* obj_button;
	}

	int load();
	int free();

	Instance* create_button(int, int, Font*, const std::string&, RGBA, std::function<void (SIDP)>);
}}

#endif // BEE_UI_H
