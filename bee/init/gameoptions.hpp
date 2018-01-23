/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_GAMEOPTIONS_H
#define BEE_INIT_GAMEOPTIONS_H 1

#include <functional>

#include "../enum.hpp"

#include "../data/variant.hpp"

namespace bee {
	struct GameOption {
		std::string name;
		Variant value;
		std::function<int (GameOption*, Variant)> setter;

		GameOption(const std::string&, Variant);
		GameOption(const std::string&, Variant, std::function<int (GameOption*, Variant)>);

		int set(Variant);
	};

	Variant get_option(const std::string&);
	int set_option(const std::string&, Variant);
	int set_option(const std::string&, Variant, std::function<int (GameOption*, Variant)>);


	template <typename T>
	int set_option(const std::string& name, T value) {
		return set_option(name, Variant(value));
	}
	template <typename T>
	int set_option(const std::string& name, T value, std::function<int (GameOption*, Variant)> setter) {
		return set_option(name, Variant(value), setter);
	}
}

#endif // BEE_INIT_GAMEOPTIONS_H
