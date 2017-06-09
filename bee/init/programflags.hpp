/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_PROGRAMFLAGS_H
#define BEE_INIT_PROGRAMFLAGS_H 1

#include <list>

namespace bee {
	struct ProgramFlags {
		std::string longopt;
		char shortopt;
		bool pre_init;
		int has_arg;
		std::function<void (char*)> func;

		ProgramFlags();
		ProgramFlags(std::string, char, bool, int, std::function<void (char*)>);
	};

	namespace internal {
		std::list<ProgramFlags*>& get_standard_flags();
	}

	int handle_flags(const std::list<ProgramFlags*>&, bool);

	std::list<ProgramFlags*> get_standard_flags();
	int free_standard_flags();
}

#endif // BEE_INIT_PROGRAMFLAGS_H
