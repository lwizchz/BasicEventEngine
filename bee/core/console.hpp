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

#include "sidp.hpp"

#include "messenger/messagecontents.hpp"

namespace bee{
	// Forward declaration
	class TextData;

	struct Console {
		bool is_open;

		std::unordered_map<std::string,std::pair<std::string,std::function<void (std::shared_ptr<MessageContents>)>>> commands;
		std::unordered_map<std::string,std::string> aliases;
		std::unordered_map<std::string,SIDP> variables;

		std::map<SDL_Keycode,std::string> bindings;

		std::string input;

		std::vector<std::string> history;
		int history_index;

		std::stringstream log;
		size_t page_index;

		std::vector<std::string> completion_commands;
		int completion_index;
		std::string input_tmp;

		// Set the drawing sizes
		unsigned int x;
		unsigned int y;
		unsigned int w;
		unsigned int h;
		unsigned int line_height;

		TextData* td_log;

		Console();
		~Console();
	};

	namespace internal {
		int console_handle_input(SDL_Event*);
		int console_init_commands();
		int console_run_internal(const std::string&, bool, Uint32);
		int console_run(const std::string&, bool, Uint32);
		int console_complete(const std::string&);
		std::vector<SIDP> console_parse_parameters(const std::string&);
		int console_draw();
	}

	int console_open();
	int console_close();
	int console_toggle();
	bool console_get_is_open();

	int console_add_command(const std::string&, const std::string&, std::function<void (std::shared_ptr<MessageContents>)>);
	int console_add_command(const std::string&, std::function<void (std::shared_ptr<MessageContents>)>);
	std::string console_bind(SDL_Keycode, const std::string&);
	std::string console_bind(SDL_Keycode);
	int console_unbind(SDL_Keycode);
	int console_unbind_all();
	int console_alias(const std::string&, const std::string&);
	const std::unordered_map<std::string,std::string>& console_get_aliases();
	int console_set_var(const std::string&, SIDP);
	SIDP console_get_var(const std::string&);

	int console_run(const std::string&);
	std::string console_get_help(const std::string&);
}

#endif // BEE_CORE_CONSOLE_H
