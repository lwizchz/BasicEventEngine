/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_H
#define BEE_NETWORK_H 1

#include <string>
#include <map>
#include <memory>

#include "client.hpp"
#include "packet.hpp"

namespace bee {
	// Forward declaration
	class Instance;
namespace net {
	namespace internal {
		int host_send_players(int);
		int host_send_data(int);
	}

	int init();
	bool get_is_initialized();
	int close();

	int send_packet(const NetworkClient&, std::unique_ptr<NetworkPacket> const &);
	std::unique_ptr<NetworkPacket> recv_packet();

	int handle_events();

	int session_start(const std::string&, int, const std::string&);
	const std::map<std::string,std::string>& session_find();
	int session_join(const std::string&, const std::string&);
	bool get_is_connected();
	int session_end();

	int session_sync_data(const std::string&, const std::string&);
	int session_sync_instance(Instance*);
	int session_sync_player(int, Instance*);

	std::string get_print();
	const std::map<int,NetworkClient>& get_players();
}}

#endif // BEE_NETWORK_H
