/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_PROGRAMFLAGS_H
#define BEE_INIT_PROGRAMFLAGS_H 1

#include <list>

#include "../enum.hpp"

namespace bee {
	struct ProgramFlag {
		std::string longopt;
		char shortopt;
		bool pre_init;
		E_FLAGARG arg_type;
		std::function<void (const std::string&)> func;

		ProgramFlag();
		ProgramFlag(const std::string&, char, bool, E_FLAGARG, const std::function<void (const std::string&)>&);
	};

	namespace internal {
		std::list<ProgramFlag*>& get_standard_flags();

		ProgramFlag* get_long_flag(const std::list<ProgramFlag*>&, const std::string&);
		ProgramFlag* get_short_flag(const std::list<ProgramFlag*>&, char);
	}

	int handle_flags(const std::list<ProgramFlag*>&, bool);

	std::list<ProgramFlag*> get_standard_flags();
	int free_standard_flags();
}

#endif // BEE_INIT_PROGRAMFLAGS_H
