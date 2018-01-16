/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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

#include <SDL2/SDL_net.h>

#include "../data/variant.hpp"

namespace bee {
	// Forward declarations
	struct NetworkClient;
	class NetworkPacket;
	class Instance;

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

		std::map<Uint16,std::unique_ptr<NetworkPacket>> buffer;
		std::map<std::string,Variant> data;
		std::map<std::string,Instance*> instances;

		NetworkConnection();

		int get_new_player_id() const;
	};
}

#endif // BEE_NETWORK_CONNECTION_H
