/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_PACKET_H
#define BEE_NETWORK_PACKET_H 1

#include <vector>
#include <memory>

#include <SDL2/SDL_net.h>

namespace bee {
	// Forward declarations
	class NetworkClient;
	class NetworkConnection;

	class NetworkPacket {
		Uint8* packet;
		size_t size;
		std::vector<Uint8> data;

		std::vector<std::pair<size_t,Uint8*>> multi_packet;

		Uint8 signals;

		int set_size(size_t);
	public:
		static size_t MAX_SIZE;

		Uint8 id;

		NetworkPacket();
		NetworkPacket(Uint8, Uint8, Uint8);
		NetworkPacket(Uint8, Uint8, Uint8, size_t, Uint8*);
		NetworkPacket(UDPpacket*);
		~NetworkPacket();
		int reset();
		int free_multi();

		int load_net(Uint8*);
		int load_data(size_t, Uint8*);
		int load_data(std::pair<size_t,Uint8*>);
		int append_data(size_t, Uint8*);
		int append_data(std::pair<size_t,Uint8*>);
		int append_net(UDPpacket*);

		Uint8* get();
		std::pair<size_t,Uint8*> get_raw();
		const std::vector<std::pair<size_t,Uint8*>>& get_multi();

		size_t get_size() const;
		Uint8 get_signal1() const;
		Uint8 get_signal2() const;
	};

	int send_packet(const NetworkClient&, std::unique_ptr<NetworkPacket> const &);
	std::unique_ptr<NetworkPacket> recv_packet(NetworkConnection*);
}

#endif // BEE_NETWORK_PACKET_H
