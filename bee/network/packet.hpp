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

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	class NetworkData;
	struct NetworkClient;
	struct NetworkConnection;

	class NetworkPacket {
		std::vector<Uint8> packet;

		NetworkData* data;
		std::vector<std::vector<Uint8>> multi_packet;

		Uint16 sequence;
		Uint32 timestamp;

		static Uint16 next_id;
		static Uint16 get_next_id();

		static bool verify_data(std::vector<Uint8>&);
	public:
		static const size_t MAX_SIZE;
		static const size_t META_SIZE;

		Uint8 id;

		NetworkPacket();
		NetworkPacket(Uint8);
		NetworkPacket(Uint8, NetworkData*);
		NetworkPacket(Uint8, E_NETSIG1, E_NETSIG2);
		NetworkPacket(Uint8, E_NETSIG1, E_NETSIG2, std::vector<Uint8>);
		NetworkPacket(const NetworkPacket&);
		NetworkPacket(UDPpacket*);
		~NetworkPacket();
		int reset();
		int free_multi();

		int load_net(UDPpacket*);
		int append_net(UDPpacket*);
		int load_data(NetworkData*);
		int append_data(NetworkData*);

		bool get_is_sequence_complete();

		std::vector<Uint8> get();
		const std::vector<std::vector<Uint8>>& get_multi();
		NetworkData* get_data();
		std::vector<Uint8> get_raw() const;

		Uint16 get_packet_id() const;
		Uint32 get_time() const;

		E_NETSIG1 get_signal1() const;
		E_NETSIG2 get_signal2() const;
		size_t get_size() const;
	};

namespace net {
	int send_packet(const NetworkClient&, std::unique_ptr<NetworkPacket> const &);
	std::unique_ptr<NetworkPacket> recv_packet(NetworkConnection*);
}}

#endif // BEE_NETWORK_PACKET_H
