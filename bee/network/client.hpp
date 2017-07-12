/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_CLIENT_H
#define BEE_NETWORK_CLIENT_H 1

#include <string>

#include <SDL2/SDL_net.h>

namespace bee {
	struct NetworkClient {
		UDPsocket sock;
		int channel;
		Uint32 last_recv;
		int id;
		std::string name;

		NetworkClient();
		NetworkClient(UDPsocket, int);
	};
}

#endif // BEE_NETWORK_CLIENT_H
