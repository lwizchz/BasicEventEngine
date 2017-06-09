/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_CORE_NETWORKDATA_H
#define BEE_CORE_NETWORKDATA_H 1

#include <string>
#include <map>

#include "networkclient.hpp"

namespace bee {
	struct NetworkData {
		bool is_initialized;
		UDPsocket udp_sock;
		UDPpacket* udp_data;

		int id;
		bool is_connected;
		bool is_host;
		int channel;

		Uint32 timeout;
		Uint32 last_recv;

		std::map<std::string,std::string> servers;

		std::string name;
		unsigned int max_players;
		int self_id;
		std::map<int,NetworkClient> players;

		Uint8* tmp_data_buffer;
		std::map<std::string,std::string> data;

		NetworkData();
	};
}

#endif // BEE_CORE_NETWORKDATA_H
