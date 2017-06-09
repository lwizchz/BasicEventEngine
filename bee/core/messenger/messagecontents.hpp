/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_MESSAGECONTENTS_H
#define BEE_CORE_MESSAGECONTENTS_H 1

#include <SDL2/SDL.h> // Include the required SDL headers

#include "../../enum.hpp"

namespace bee {
	struct MessageContents {
		bool has_processed;
		Uint32 tickstamp;
		std::vector<std::string> tags;
		E_MESSAGE type;
		std::string descr;
		std::shared_ptr<void> data;

		MessageContents();
		MessageContents(Uint32, std::vector<std::string>, E_MESSAGE, std::string, std::shared_ptr<void>);
	};
}

#endif // BEE_CORE_MESSAGECONTENTS_H
