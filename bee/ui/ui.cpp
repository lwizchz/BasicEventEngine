/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UI
#define BEE_UI

#include <set>
#include <vector>

#include "ui.hpp"

#include "../messenger/messenger.hpp"

#include "../core/instance.hpp"
#include "../core/rooms.hpp"

#include "../resource/sound.hpp"
#include "../resource/font.hpp"
#include "../resource/object.hpp"
#include "../resource/room.hpp"

#include "../resources/headers.hpp"

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
		Object* obj_slider = nullptr;
		Object* obj_optionbox = nullptr;

		std::map<Instance*,std::set<Instance*>> parents;
		std::map<Instance*,std::function<void (Instance*)>> button_callbacks;
		std::map<Instance*,std::function<void (Instance*, const std::string&)>> text_entry_callbacks;
		std::map<Instance*,std::function<std::vector<Variant> (Instance*, const std::string&)>> text_entry_completors;
		std::map<Instance*,std::function<void (Instance*, const std::string&, const SDL_Event*)>> text_entry_handlers;
		std::map<Instance*,std::function<void (Instance*, int)>> slider_callbacks;
		std::map<Instance*,std::vector<std::function<void (Instance*, bool)>>> optionbox_callbacks;
	}

	int load() {
		if (internal::is_loaded) {
			return 1;
		}

		// Load sounds
		internal::snd_button_press = Sound::add("snd_ui_button_press", "/bee/resources/sounds/ui/button_press.wav", false);
		internal::snd_button_release = Sound::add("snd_ui_button_release", "/bee/resources/sounds/ui/button_release.wav", false);

		// Load objects
		internal::obj_button = new ObjUIButton();
		internal::obj_handle = new ObjUIHandle();
		internal::obj_text_entry = new ObjUITextEntry();
		internal::obj_gauge = new ObjUIGauge();
		internal::obj_slider = new ObjUISlider();
		internal::obj_optionbox = new ObjUIOptionBox();

		internal::is_loaded = true;

		if ((internal::snd_button_press == nullptr)||(internal::snd_button_release == nullptr)) { // Output sound error after loading the UI elements
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "Failed to load UI sounds");
			return 2;
		}

		return 0;
	}
	#define DEL(x) if (x!=nullptr) {delete x; x=nullptr;}
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
		DEL(internal::obj_slider);
		DEL(internal::obj_optionbox);

		internal::is_loaded = false;

		return 0;
	}
	#undef DEL

	int destroy_parent(bee::Instance* parent) {
		for (auto& i : internal::parents[parent]) {
			ObjUIHandle* obj_handle = static_cast<ObjUIHandle*>(i->get_object());
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
		button->set_corner(x, y);

		button->set_data("font", static_cast<void*>(font));
		button->set_data("text", str);

		ObjUIButton* obj_button = static_cast<ObjUIButton*>(internal::obj_button);
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

		std::map<Instance*,std::function<void (Instance*)>>::iterator b (internal::button_callbacks.find(button));
		if (b == internal::button_callbacks.end()) {
			return 2;
		}

		std::function<void (Instance*)> func = b->second;
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
		handle->set_corner(x, y);

		handle->set_data("w", w);
		handle->set_data("h", h);

		ObjUIHandle* obj_handle = static_cast<ObjUIHandle*>(internal::obj_handle);
		obj_handle->update(handle);
		obj_handle->bind(handle, parent);

		internal::parents[parent].insert(handle);

		return handle;
	}
	int destroy_handle(Instance* handle) {
		Instance* parent = static_cast<Instance*>(handle->get_data("parent").p);
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
		text_entry->set_corner(x, y);

		ObjUITextEntry* obj_text_entry = static_cast<ObjUITextEntry*>(internal::obj_text_entry);
		obj_text_entry->update(text_entry);
		obj_text_entry->set_size(text_entry, rows, cols);

		internal::text_entry_callbacks.emplace(text_entry, func);

		return text_entry;
	}
	int add_text_entry_completor(Instance* text_entry, std::function<std::vector<Variant> (Instance*, const std::string&)> func) {
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

		std::map<Instance*,std::function<void (Instance*, const std::string&)>>::iterator te (internal::text_entry_callbacks.find(text_entry));
		if (te == internal::text_entry_callbacks.end()) {
			return 2;
		}

		std::function<void (Instance*, const std::string&)> func = te->second;
		if (func == nullptr) {
			return 3;
		}

		func(text_entry, input);

		return 0;
	}
	std::vector<Variant> text_entry_completor(Instance* text_entry, const std::string& input) {
		std::vector<Variant> uncomplete = {Variant(input)};

		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: text entry completor not run");
			return uncomplete;
		}

		std::map<Instance*,std::function<std::vector<Variant> (Instance*, const std::string&)>>::iterator tec (internal::text_entry_completors.find(text_entry));
		if (tec == internal::text_entry_completors.end()) {
			return uncomplete;
		}

		std::function<std::vector<Variant> (Instance*, const std::string&)> func = tec->second;
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

		std::map<Instance*,std::function<void (Instance*, const std::string&, const SDL_Event*)>>::iterator teh (internal::text_entry_handlers.find(text_entry));
		if (teh == internal::text_entry_handlers.end()) {
			return 2;
		}

		std::function<void (Instance*, const std::string&, const SDL_Event*)> func = teh->second;
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
		gauge->set_corner(x, y);

		gauge->set_data("w", w);
		gauge->set_data("h", h);

		ObjUIGauge* obj_gauge = static_cast<ObjUIGauge*>(internal::obj_gauge);
		obj_gauge->update(gauge);
		if (range > 0) {
			obj_gauge->set_range(gauge, range);
		} else {
			obj_gauge->start_pulse(gauge);
		}

		return gauge;
	}

	Instance* create_slider(int x, int y, int w, int h, int range, int value, bool is_continuous_callback, std::function<void (Instance*, int)> func) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: slider not created");
			return nullptr;
		}

		bee::Instance* slider = bee::get_current_room()->add_instance(-1, internal::obj_slider, x, y, 0.0);
		slider->set_corner(x, y);

		slider->set_data("w", w);
		slider->set_data("h", h);

		ObjUISlider* obj_slider = static_cast<ObjUISlider*>(internal::obj_slider);
		obj_slider->update(slider);
		obj_slider->set_range(slider, range);
		obj_slider->set_value(slider, value);
		obj_slider->set_is_continuous(slider, is_continuous_callback);

		internal::slider_callbacks.emplace(slider, func);

		return slider;
	}
	int slider_callback(Instance* slider, int value) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: slider callback not run");
			return 1;
		}

		std::map<Instance*,std::function<void (Instance*, int)>>::iterator sl (internal::slider_callbacks.find(slider));
		if (sl == internal::slider_callbacks.end()) {
			return 2;
		}

		std::function<void (Instance*, int)> func = sl->second;
		if (func == nullptr) {
			return 3;
		}

		func(slider, value);

		return 0;
	}

	Instance* create_optionbox(int x, int y, int w, int h) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: optionbox not created");
			return nullptr;
		}

		bee::Instance* optionbox = bee::get_current_room()->add_instance(-1, internal::obj_optionbox, x, y, 0.0);
		optionbox->set_corner(x, y);

		optionbox->set_data("w", w);
		optionbox->set_data("h", h);
		if (h < 0) {
			optionbox->set_data("adaptive_height", true);
		}

		return optionbox;
	}
	int push_optionbox_option(Instance* optionbox, std::function<void (Instance*, bool)> callback) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: option not added");
			return 1;
		}

		internal::optionbox_callbacks[optionbox].push_back(callback);

		return 0;
	}
	int pop_optionbox_option(Instance* optionbox) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: option not removed");
			return 1;
		}

		if (internal::optionbox_callbacks[optionbox].empty()) {
			return 1;
		}

		internal::optionbox_callbacks[optionbox].pop_back();

		return 0;
	}
	int reset_optionbox_options(Instance* optionbox) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: optionbox not reset");
			return 1;
		}

		std::map<Instance*,std::vector<std::function<void (Instance*, bool)>>>::iterator opb (internal::optionbox_callbacks.find(optionbox));
		if (opb == internal::optionbox_callbacks.end()) {
			return 2;
		}

		internal::optionbox_callbacks.erase(opb);

		return 0;
	}
	int optionbox_callback(Instance* optionbox, size_t option_index, bool state) {
		if (!internal::is_loaded) {
			messenger::send({"engine", "ui"}, E_MESSAGE::WARNING, "UI not initialized: optionbox callback not run");
			return 1;
		}

		std::map<Instance*,std::vector<std::function<void (Instance*, bool)>>>::iterator opb (internal::optionbox_callbacks.find(optionbox));
		if (
			(opb == internal::optionbox_callbacks.end())
			||(option_index >= opb->second.size())
		) {
			return 2;
		}

		std::function<void (Instance*, bool)> func = opb->second.at(option_index);
		if (func == nullptr) {
			return 3;
		}

		func(optionbox, state);

		return 0;
	}
}}

#endif // BEE_UI
