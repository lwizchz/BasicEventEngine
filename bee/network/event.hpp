/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_EVENT_H
#define BEE_NETWORK_EVENT_H 1

#include "../enum.hpp"

namespace bee {
	struct NetworkEvent {
		E_NETEVENT type;

		NetworkEvent();
	};
}

#endif // BEE_NETWORK_EVENT_H
