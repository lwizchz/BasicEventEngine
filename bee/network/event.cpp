/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_EVENT
#define BEE_NETWORK_EVENT 1

#include "event.hpp"

namespace bee {
	NetworkEvent::NetworkEvent() :
		type(E_NETEVENT::NONE),
		id(-1),
		data()
	{}
	NetworkEvent::NetworkEvent(E_NETEVENT new_type) :
		type(new_type),
		id(-1),
		data()
	{}
}

#endif // BEE_NETWORK_EVENT
