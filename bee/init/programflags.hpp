/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_PROGRAMFLAGS_H
#define BEE_INIT_PROGRAMFLAGS_H 1

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
}

#endif // BEE_INIT_PROGRAMFLAGS_H
