/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI
#define BEE_UI

#include <set>

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
		Sprite* spr_handle = nullptr;

		// Declare sounds
		Sound* snd_button_press = nullptr;
		Sound* snd_button_release = nullptr;

		// Declare objects
		Object* obj_button = nullptr;
		Object* obj_handle = nullptr;

		std::map<Instance*,std::set<Instance*>> parents;
		std::map<Instance*,std::function<void (Instance*)>> button_callbacks;
	}

	int load() {
		if (internal::is_loaded) {
			return 1;
		}

		// Load sprites
		internal::spr_button = new Sprite("spr_ui_button", "ui/button.png");
		internal::spr_handle = new Sprite("spr_ui_handle", "ui/handle.png");

		// Load sounds
		internal::snd_button_press = new Sound("snd_ui_button_press", "ui/button_press.wav", false);
			internal::snd_button_press->load();
		internal::snd_button_release = new Sound("snd_ui_button_release", "ui/button_release.wav", false);
			internal::snd_button_release->load();

		// Load objects
		internal::obj_button = new ObjUIButton();
			internal::obj_button->set_sprite(internal::spr_button);
		internal::obj_handle = new ObjUIHandle();
			internal::obj_handle->set_sprite(internal::spr_handle);

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
		DEL(internal::spr_handle);

		// Free sounds
		DEL(internal::snd_button_press);
		DEL(internal::snd_button_release);

		// Free objects
		DEL(internal::obj_button);
		DEL(internal::obj_handle);

		internal::is_loaded = false;

		return 0;
	}
	#undef DEL

	int destroy_parent(bee::Instance* parent) {
		for (auto& i : internal::parents[parent]) {
			ObjUIHandle* obj_handle = dynamic_cast<ObjUIHandle*>(i->get_object());
			obj_handle->update(i);
			obj_handle->bind(i, nullptr);

			get_current_room()->destroy(i);
		}
		internal::parents.erase(parent);
		return 0;
	}

	Instance* create_button(int x, int y, Font* font, const std::string& str, RGBA color, std::function<void (bee::Instance*)> func) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: button not created");
			return nullptr;
		}

		if (!internal::spr_button->get_is_loaded()) {
			internal::spr_button->load();
		}

		bee::Instance* button = bee::get_current_room()->add_instance(-1, internal::obj_button, x, y, 0.0);
		button->set_corner_x(x);
		button->set_corner_y(y);

		button->set_data("font", static_cast<void*>(font));
		button->set_data("text", str);

		ObjUIButton* obj_button = dynamic_cast<ObjUIButton*>(internal::obj_button);
		obj_button->update(button);
		obj_button->center_width(button);

		button->set_data("color_r", color.r);
		button->set_data("color_g", color.g);
		button->set_data("color_b", color.b);
		button->set_data("color_a", color.a);

		internal::button_callbacks.emplace(button, func);

		return button;
	}
	int button_callback(Instance* inst) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: button callback not run");
			return 1;
		}

		if (internal::button_callbacks.find(inst) == internal::button_callbacks.end()) {
			return 2;
		}

		std::function<void (Instance*)> func = internal::button_callbacks[inst];
		if (func == nullptr) {
			return 3;
		}

		internal::button_callbacks[inst](inst);

		return 0;
	}

	Instance* create_handle(int x, int y, int w, int h, RGBA color, Instance* parent) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: handle not created");
			return nullptr;
		}

		if (!internal::spr_handle->get_is_loaded()) {
			internal::spr_handle->load();
		}

		bee::Instance* handle = bee::get_current_room()->add_instance(-1, internal::obj_handle, x, y, 0.0);
		handle->set_corner_x(x);
		handle->set_corner_y(y);

		handle->set_data("w", w);
		handle->set_data("h", h);

		handle->set_data("color_r", color.r);
		handle->set_data("color_g", color.g);
		handle->set_data("color_b", color.b);
		handle->set_data("color_a", color.a);

		ObjUIHandle* obj_handle = dynamic_cast<ObjUIHandle*>(internal::obj_handle);
		obj_handle->update(handle);
		obj_handle->bind(handle, parent);

		internal::parents[parent].insert(handle);

		return handle;
	}
	int destroy_handle(Instance* handle) {
		Instance* parent = static_cast<Instance*>(SIDP_p(handle->get_data("parent")));
		if (parent != nullptr) {
			if (internal::parents.find(parent) != internal::parents.end()) {
				internal::parents.at(parent).erase(handle);
			}
		}
		return 0;
	}
}}

#endif // BEE_UI
