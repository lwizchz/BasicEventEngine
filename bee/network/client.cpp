/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_CLIENT
#define BEE_NETWORK_CLIENT 1

#include "client.hpp"

namespace bee {
	NetworkClient::NetworkClient() :
		sock(nullptr),
		channel(-1),
		last_recv(0)
	{}
}

#endif // BEE_NETWORK_CLIENT
