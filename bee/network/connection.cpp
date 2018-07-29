/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_CONNECTION
#define BEE_NETWORK_CONNECTION 1

#include "connection.hpp"

#include "client.hpp"
#include "packet.hpp"

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

		buffer(),
		data(),
		instances()
	{}

	int NetworkConnection::get_new_player_id() const {
		for (size_t i=1; i<max_players; ++i) {
			if (players.find(i) == players.end()) {
				return i;
			}
		}
		return -1;
	}
}

#endif // BEE_NETWORK_CONNECTION
