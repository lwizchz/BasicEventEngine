/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_CORE_NETWORKDATA
#define _BEE_CORE_NETWORKDATA 1

#include "networkdata.hpp"

namespace bee {
	NetworkData::NetworkData() :
		is_initialized(false),
		udp_sock(nullptr),
		udp_data(nullptr),

		id(3054),
		is_connected(false),
		is_host(false),
		channel(-1),

		timeout(1000),
		last_recv(0),

		servers(),

		name(),
		max_players(0),
		self_id(-1),
		players(),

		tmp_data_buffer(nullptr),
		data()
	{}
}

#endif // _BEE_CORE_NETWORKDATA
