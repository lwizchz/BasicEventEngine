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
#include "../resource/sound.hpp"
#include "../resource/font.hpp"
#include "../resource/object.hpp"

namespace bee { namespace ui {
	namespace internal {
		bool is_loaded = false;

		// Declare sprites
		Sprite* spr_button = nullptr;

		// Declare sounds
		Sound* snd_button_press = nullptr;
		Sound* snd_button_release = nullptr;

		// Declare objects
		Object* obj_button = nullptr;

		std::map<Instance*,std::function<void (Instance*)>> callbacks;
	}

	int load() {
		if (internal::is_loaded) {
			return 1;
		}

		// Load sprites
		internal::spr_button = new Sprite("spr_ui_button", "ui/button.png");

		// Load sounds
		internal::snd_button_press = new Sound("snd_ui_button_press", "ui/button_press.wav", false);
			internal::snd_button_press->load();
		internal::snd_button_release = new Sound("snd_ui_button_release", "ui/button_release.wav", false);
			internal::snd_button_release->load();

		// Load objects
		internal::obj_button = new ObjUIButton();
			internal::obj_button->set_sprite(internal::spr_button);

		internal::is_loaded = true;

		return 0;
	}
	#define DEL(x) delete x; x=nullptr
	int free() {
		if (!internal::is_loaded) {
			return 1;
		}

		// Free sprites
		DEL(internal::spr_button);

		// Free sounds
		DEL(internal::snd_button_press);
		DEL(internal::snd_button_release);

		// Free objects
		DEL(internal::obj_button);

		internal::is_loaded = false;

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

		//SIDP f (std::move(func));
		//button->set_data("press_func", f);
		internal::callbacks.emplace(button, func);

		return button;
	}
	int button_callback(Instance* inst) {
		if (internal::callbacks.find(inst) == internal::callbacks.end()) {
			return 1;
		}

		std::function<void (Instance*)> func = internal::callbacks[inst];
		if (func == nullptr) {
			return 2;
		}

		internal::callbacks[inst](inst);

		return 0;
	}
}}

#endif // BEE_UI
