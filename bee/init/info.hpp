/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_INFO_H
#define BEE_INIT_INFO_H 1

#include <string>

namespace bee {
	struct VersionInfo {
		unsigned int major;
		unsigned int minor;
		unsigned int patch;

		std::string to_str() const;
	};

	std::string get_usage_text();

	std::string get_build_id();
	unsigned int get_game_id();
	std::string get_game_name();

	VersionInfo get_engine_version();
	VersionInfo get_game_version();
}

#endif // BEE_INIT_INFO_H
