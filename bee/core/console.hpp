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
}

#endif // BEE_CORE_CONSOLE_H
