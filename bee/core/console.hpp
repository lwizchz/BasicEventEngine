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

#include "sidp.hpp"

namespace bee{
	// Forward declaration
	class Instance;
	struct KeyBind;

namespace console {
	int reset();

	namespace internal {
		int init();
		int init_ui();
		int init_commands();
		int close();

		int update_ui();

		int handle_input(SDL_Event*);
		int run_internal(const std::string&, bool, Uint32);
		int run(const std::string&, bool, Uint32);
		std::vector<SIDP> complete(Instance*, const std::string&);
		std::vector<SIDP> parse_parameters(const std::string&, bool);
		std::string replace_vars(const std::string&);
		int draw();
	}

	int open();
	int close();
	int toggle();
	bool get_is_open();

	int add_command(const std::string&, const std::string&, std::function<void (const MessageContents&)>);
	int add_command(const std::string&, std::function<void (const MessageContents&)>);

	int bind(SDL_Keycode, KeyBind);
	int add_keybind(SDL_Keycode, KeyBind, std::function<void (const MessageContents&)>);
	KeyBind get_keybind(SDL_Keycode);
	SDL_Keycode get_keycode(const std::string&);
	int unbind(SDL_Keycode);
	int unbind(KeyBind, bool);
	int unbind_all();

	int alias(const std::string&, const std::string&);
	const std::unordered_map<std::string,std::string>& get_aliases();

	int set_var(const std::string&, const SIDP&);
	SIDP get_var(const std::string&);

	int run(const std::string&);
	std::string get_help(const std::string&);
}}

#endif // BEE_CORE_CONSOLE_H
