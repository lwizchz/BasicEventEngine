/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_EVENT_H
#define BEE_NETWORK_EVENT_H 1

#include <map>

#include <SDL2/SDL.h> // Include the SDL2 headers for the Uint8 type

#include "../enum.hpp"

#include "../data/variant.hpp"

namespace bee {
	struct NetworkEvent {
		E_NETEVENT type;
		int id;
		std::map<std::string,Variant> data;
		std::map<std::string,std::vector<Uint8>> instances;

		NetworkEvent();
		explicit NetworkEvent(E_NETEVENT);
	};
}

#endif // BEE_NETWORK_EVENT_H
