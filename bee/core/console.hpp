/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_CONSOLE_H
#define BEE_CORE_CONSOLE_H 1

#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <memory>
#include <sstream>

#include "../messenger/messagecontents.hpp"

#include "../data/sidp.hpp"

namespace bee{
	// Forward declaration
	class Instance;
	struct KeyBind;

namespace console {
	void reset();

	namespace internal {
		int init();
		int init_ui();
		void close();

		void update_ui();

		void handle_input(SDL_Event*);
		int run(const std::string&, bool);
		//std::vector<Variant> complete(Instance*, const std::string&);
		void draw();
	}

	void open();
	void close();
	void toggle();
	bool get_is_open();
	void clear();

	int add_command(const std::string&, const std::string&, std::function<void (const MessageContents&)>);

	int bind(SDL_Keycode, KeyBind);
	int add_keybind(SDL_Keycode, KeyBind, std::function<void (const MessageContents&)>);
	KeyBind get_keybind(SDL_Keycode);
	SDL_Keycode get_keycode(const std::string&);
	void unbind(SDL_Keycode);
	int unbind(KeyBind);
	void unbind_all();

	//int set_var(const std::string&, const Variant&);
	//Variant get_var(const std::string&);

	int run(const std::string&);
	std::string get_help(const std::string&);
	void log(E_MESSAGE, const std::string&);
}}

#endif // BEE_CORE_CONSOLE_H
