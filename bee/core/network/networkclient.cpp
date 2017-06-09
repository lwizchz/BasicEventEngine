/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_NETWORKCLIENT
#define BEE_CORE_NETWORKCLIENT 1

#include "networkclient.hpp"

namespace bee {
	NetworkClient::NetworkClient() :
		sock(nullptr),
		channel(-1),
		last_recv(0)
	{}
}

#endif // BEE_CORE_NETWORKCLIENT
