/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_NETWORKCLIENT_H
#define _BEE_CORE_NETWORKCLIENT_H 1

#include "../../engine.hpp"

namespace bee {
	struct NetworkClient {
		UDPsocket sock;
		int channel;
		Uint32 last_recv;

		NetworkClient();
	};
}

#endif // _BEE_CORE_NETWORKCLIENT_H
