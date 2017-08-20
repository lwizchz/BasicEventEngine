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
	int init();
	bool get_is_initialized();
	Uint32 get_time();
	int close();

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

	namespace internal {
		int host_handle_packet(std::unique_ptr<NetworkPacket> const &);
		int host_send_players(int);
		int host_send_data(int);
		int host_send_keepalive();

		int client_handle_packet(std::unique_ptr<NetworkPacket> const &);
		int client_send_data();
		int client_send_keepalive();

		int destroy_instance(Instance*);
	}
}}

#endif // BEE_NETWORK_H
