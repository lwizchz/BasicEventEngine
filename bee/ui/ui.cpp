/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI
#define BEE_UI

#include "ui.hpp"
#include "widgets.hpp"

#include "../core/instance.hpp"

#include "../resource/sprite.hpp"
#include "../resource/font.hpp"
#include "../resource/object.hpp"

namespace bee { namespace ui {
	namespace internal {
		// Declare sprites
		Sprite* spr_button = nullptr;

		// Declare objects
		Object* obj_button = nullptr;
	}

	int load() {
		// Load sprites
		internal::spr_button = new Sprite("spr_ui_button", "ui/button.png");

		// Load objects
		internal::obj_button = new ObjUIButton();
			internal::obj_button->set_sprite(internal::spr_button);

		return 0;
	}
	#define DEL(x) delete x; x=nullptr
	int free() {
		// Free sprites
		DEL(internal::spr_button);

		// Free objects
		DEL(internal::obj_button);

		return 0;
	}
	#undef DEL

	Instance* create_button(int x, int y, bee::Font* font, const std::string& str, bee::RGBA color, std::function<void (bee::SIDP)> func) {
		internal::spr_button->load();

		bee::Instance* button = bee::get_current_room()->add_instance(-1, internal::obj_button, x, y, 0.0);
		button->set_data("font", (void*)font);
		button->set_data("text", str);
		((ObjUIButton*)button->get_object())->center_width(button);

		button->set_data("color_r", color.r);
		button->set_data("color_g", color.g);
		button->set_data("color_b", color.b);
		button->set_data("color_a", color.a);

		bee::SIDP f;
		f.function(func);
		button->set_data("press_func", f);

		return button;
	}
}}

#endif // BEE_UI
