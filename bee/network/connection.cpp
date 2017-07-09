/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_CONNECTION
#define BEE_NETWORK_CONNECTION 1

#include "connection.hpp"

namespace bee {
	NetworkConnection::NetworkConnection() :
		udp_sock(nullptr),
		udp_data(nullptr),

		is_connected(false),
		is_host(false),
		channel(-1),

		last_recv(0),

		name(),
		max_players(0),
		self_id(-1),
		players(),

		tmp_data_buffer(nullptr),
		data()
	{}
}

#endif // BEE_NETWORK_CONNECTION
