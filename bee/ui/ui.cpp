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
#include <vector>

#include "ui.hpp"
#include "elements.hpp"

#include "../messenger/messenger.hpp"

#include "../core/instance.hpp"
#include "../core/room.hpp"

#include "../resource/sprite.hpp"
#include "../resource/sound.hpp"
#include "../resource/font.hpp"
#include "../resource/object.hpp"
#include "../resource/room.hpp"

namespace bee { namespace ui {
	namespace internal {
		bool is_loaded = false;

		// Declare sounds
		Sound* snd_button_press = nullptr;
		Sound* snd_button_release = nullptr;

		// Declare objects
		Object* obj_button = nullptr;
		Object* obj_handle = nullptr;
		Object* obj_text_entry = nullptr;
		Object* obj_gauge = nullptr;

		std::map<Instance*,std::set<Instance*>> parents;
		std::map<Instance*,std::function<void (Instance*)>> button_callbacks;
		std::map<Instance*,std::function<void (Instance*, const std::string&)>> text_entry_callbacks;
		std::map<Instance*,std::function<std::vector<SIDP> (Instance*, const std::string&)>> text_entry_completors;
		std::map<Instance*,std::function<void (Instance*, const std::string&, const SDL_Event*)>> text_entry_handlers;
	}

	int load() {
		if (internal::is_loaded) {
			return 1;
		}

		// Load sounds
		internal::snd_button_press = new Sound("snd_ui_button_press", "ui/button_press.wav", false);
			internal::snd_button_press->load();
		internal::snd_button_release = new Sound("snd_ui_button_release", "ui/button_release.wav", false);
			internal::snd_button_release->load();

		// Load objects
		internal::obj_button = new ObjUIButton();
		internal::obj_handle = new ObjUIHandle();
		internal::obj_text_entry = new ObjUITextEntry();
		internal::obj_gauge = new ObjUIGauge();

		internal::is_loaded = true;

		return 0;
	}
	#define DEL(x) delete x; x=nullptr
	int free() {
		if (!internal::is_loaded) {
			return 1;
		}

		// Free sounds
		DEL(internal::snd_button_press);
		DEL(internal::snd_button_release);

		// Free objects
		DEL(internal::obj_button);
		DEL(internal::obj_handle);
		DEL(internal::obj_text_entry);
		DEL(internal::obj_gauge);

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

	Instance* create_button(int x, int y, Font* font, const std::string& str, std::function<void (bee::Instance*)> func) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: button not created");
			return nullptr;
		}

		bee::Instance* button = bee::get_current_room()->add_instance(-1, internal::obj_button, x, y, 0.0);
		button->set_corner_x(x);
		button->set_corner_y(y);

		button->set_data("font", static_cast<void*>(font));
		button->set_data("text", str);

		ObjUIButton* obj_button = dynamic_cast<ObjUIButton*>(internal::obj_button);
		obj_button->update(button);
		obj_button->center_width(button);

		internal::button_callbacks.emplace(button, func);

		return button;
	}
	int button_callback(Instance* button) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: button callback not run");
			return 1;
		}

		if (internal::button_callbacks.find(button) == internal::button_callbacks.end()) {
			return 2;
		}

		std::function<void (Instance*)> func = internal::button_callbacks[button];
		if (func == nullptr) {
			return 3;
		}

		func(button);

		return 0;
	}

	Instance* create_handle(int x, int y, int w, int h, Instance* parent) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: handle not created");
			return nullptr;
		}

		bee::Instance* handle = bee::get_current_room()->add_instance(-1, internal::obj_handle, x, y, 0.0);
		handle->set_corner_x(x);
		handle->set_corner_y(y);

		handle->set_data("w", w);
		handle->set_data("h", h);

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

	Instance* create_text_entry(int x, int y, int rows, int cols, std::function<void (Instance*, const std::string&)> func) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: text entry not created");
			return nullptr;
		}

		bee::Instance* text_entry = bee::get_current_room()->add_instance(-1, internal::obj_text_entry, x, y, 0.0);
		text_entry->set_corner_x(x);
		text_entry->set_corner_y(y);

		ObjUITextEntry* obj_text_entry = dynamic_cast<ObjUITextEntry*>(internal::obj_text_entry);
		obj_text_entry->update(text_entry);
		obj_text_entry->set_size(text_entry, rows, cols);

		internal::text_entry_callbacks.emplace(text_entry, func);

		return text_entry;
	}
	int add_text_entry_completor(Instance* text_entry, std::function<std::vector<SIDP> (Instance*, const std::string&)> func) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: text entry completor not added");
			return 1;
		}

		internal::text_entry_completors.emplace(text_entry, func);

		return 0;
	}
	int add_text_entry_handler(Instance* text_entry, std::function<void (Instance*, const std::string&, const SDL_Event*)> func) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: text entry handler not added");
			return 1;
		}

		internal::text_entry_handlers.emplace(text_entry, func);

		return 0;
	}
	int text_entry_callback(Instance* text_entry, const std::string& input) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: text entry callback not run");
			return 1;
		}

		if (internal::text_entry_callbacks.find(text_entry) == internal::text_entry_callbacks.end()) {
			return 2;
		}

		std::function<void (Instance*, const std::string&)> func = internal::text_entry_callbacks[text_entry];
		if (func == nullptr) {
			return 3;
		}

		func(text_entry, input);

		return 0;
	}
	std::vector<SIDP> text_entry_completor(Instance* text_entry, const std::string& input) {
		std::vector<SIDP> uncomplete = {input};

		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: text entry completor not run");
			return uncomplete;
		}

		if (internal::text_entry_completors.find(text_entry) == internal::text_entry_completors.end()) {
			return uncomplete;
		}

		std::function<std::vector<SIDP> (Instance*, const std::string&)> func = internal::text_entry_completors[text_entry];
		if (func == nullptr) {
			return uncomplete;
		}

		return func(text_entry, input);
	}
	int text_entry_handler(Instance* text_entry, const std::string& input, const SDL_Event* e) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: text entry handler not run");
			return 1;
		}

		if (internal::text_entry_handlers.find(text_entry) == internal::text_entry_handlers.end()) {
			return 2;
		}

		std::function<void (Instance*, const std::string&, const SDL_Event*)> func = internal::text_entry_handlers[text_entry];
		if (func == nullptr) {
			return 3;
		}

		func(text_entry, input, e);

		return 0;
	}

	Instance* create_gauge(int x, int y, int w, int h, int range) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: gauge not created");
			return nullptr;
		}

		bee::Instance* gauge = bee::get_current_room()->add_instance(-1, internal::obj_gauge, x, y, 0.0);
		gauge->set_corner_x(x);
		gauge->set_corner_y(y);

		gauge->set_data("w", w);
		gauge->set_data("h", h);

		ObjUIGauge* obj_gauge = dynamic_cast<ObjUIGauge*>(internal::obj_gauge);
		obj_gauge->update(gauge);
		if (range > 0) {
			obj_gauge->set_range(gauge, range);
		} else {
			obj_gauge->start_pulse(gauge);
		}

		return gauge;
	}
}}

#endif // BEE_UI
