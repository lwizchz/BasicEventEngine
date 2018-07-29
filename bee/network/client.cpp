/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_CLIENT
#define BEE_NETWORK_CLIENT 1

#include "client.hpp"

namespace bee {
	NetworkClient::NetworkClient() :
		sock(nullptr),
		channel(-1),
		last_recv(0),
		id(-1),
		name()
	{}
	NetworkClient::NetworkClient(UDPsocket _sock, int _channel) :
		sock(_sock),
		channel(_channel),
		last_recv(0),
		id(-1),
		name()
	{}
}

#endif // BEE_NETWORK_CLIENT
