/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_CONNECTION_H
#define BEE_NETWORK_CONNECTION_H 1

#include <string>
#include <map>
#include <memory>

#include "../core/sidp.hpp"

#include "client.hpp"
#include "packet.hpp"

namespace bee {
	struct NetworkConnection {
		UDPsocket udp_sock;
		UDPpacket* udp_data;

		bool is_connected;
		bool is_host;
		int channel;

		Uint32 last_recv;

		std::string name;
		unsigned int max_players;
		int self_id;
		std::map<int,NetworkClient> players;

		std::unique_ptr<NetworkPacket> tmp_data_buffer;
		std::map<std::string,SIDP> data;

		NetworkConnection();
	};
}

#endif // BEE_NETWORK_CONNECTION_H
