/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_CONSOLE_H
#define BEE_CORE_CONSOLE_H 1

#include <functional>

#include "../messenger/messagecontents.hpp"

#include "../data/variant.hpp"

namespace bee{
	// Forward declaration
	class Instance;

namespace console {
	void reset();

	namespace internal {
		int init();
		int init_ui();
		void close();

		void update_ui();

		void handle_input(const SDL_Event*);
		int run(const std::string&, bool);
		std::vector<Variant> complete(Instance*, const std::string&);
		void draw();
	}

	void open();
	void close();
	void toggle();
	bool get_is_open();
	void clear();

	void set_var(const std::string&, const Variant&);
	Variant get_var(const std::string&);

	int run(const std::string&);
	void log(E_MESSAGE, const std::string&);
}}

#endif // BEE_CORE_CONSOLE_H
