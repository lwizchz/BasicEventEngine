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
	class SIDP;
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

	Instance* create_button(int, int, Font*, const std::string&, std::function<void (Instance*)>);
	int button_callback(Instance*);

	Instance* create_handle(int, int, int, int, Instance*);
	int destroy_handle(Instance*);

	Instance* create_text_entry(int, int, int, int, std::function<void (Instance*, const std::string&)>);
	int add_text_entry_completor(Instance*, std::function<std::vector<SIDP> (Instance*, const std::string&)>);
	int add_text_entry_handler(Instance*, std::function<void (Instance*, const std::string&, const SDL_Event*)>);
	int text_entry_callback(Instance*, const std::string&);
	std::vector<SIDP> text_entry_completor(Instance*, const std::string&);
	int text_entry_handler(Instance*, const std::string&, const SDL_Event*);

	Instance* create_gauge(int, int, int, int, int);

	Instance* create_slider(int, int, int, int, int, int, bool, std::function<void (Instance*, int)>);
	int slider_callback(Instance*, int);

	Instance* create_optionbox(int, int, int, int);
	int push_optionbox_option(Instance*, std::function<void (Instance*, bool)>);
	int pop_optionbox_option(Instance*);
	int reset_optionbox_options(Instance*);
	int optionbox_callback(Instance*, int, bool);
}}

#endif // BEE_UI_H
