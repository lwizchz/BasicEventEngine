/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_DATA_H
#define BEE_NETWORK_DATA_H 1

#include <vector>

#include <SDL2/SDL_net.h>

#include "../enum.hpp"

namespace bee {
	// Forward declarations
	class NetworkClient;
	class NetworkConnection;

	class NetworkData {
		Uint8 signals;
		std::vector<Uint8> data;
	public:
		NetworkData();
		NetworkData(Uint8);
		NetworkData(E_NETSIG1, E_NETSIG2);
		NetworkData(E_NETSIG1, E_NETSIG2, const std::vector<Uint8>&);
		~NetworkData();
		int reset();

		int append_data(const std::vector<Uint8>&);
		int append_data(const std::vector<Uint8>&, size_t);

		std::vector<Uint8> get();

		Uint8 get_signals() const;
		E_NETSIG1 get_signal1() const;
		E_NETSIG2 get_signal2() const;
	};
}

#endif // BEE_NETWORK_DATA_H
