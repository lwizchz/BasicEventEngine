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

#include <SDL2/SDL_net.h>

namespace bee {
	class NetworkPacket {
		Uint8* packet;
		size_t size;
		std::vector<Uint8> data;

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

		int load_net(Uint8*);
		int load_data(size_t, Uint8*);
		int append_data(size_t, Uint8*);
		int append_net(UDPpacket*);

		Uint8* get();
		std::pair<size_t,Uint8*> get_raw();

		size_t get_size() const;
		Uint8 get_signal1() const;
		Uint8 get_signal2() const;
	};
}

#endif // BEE_NETWORK_PACKET_H
