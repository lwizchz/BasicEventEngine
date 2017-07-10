/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_PACKET
#define BEE_NETWORK_PACKET 1

#include "packet.hpp"

#include "../util/platform.hpp"

#include "../messenger/messenger.hpp"

namespace bee {
	size_t NetworkPacket::MAX_SIZE = 65536;

	NetworkPacket::NetworkPacket() :
		packet(new Uint8[1]),
		size(0),
		data(),
		signals(0),
		id(0)
	{
		set_size(4);
	}
	NetworkPacket::NetworkPacket(Uint8 new_id, Uint8 signal1, Uint8 signal2) :
		packet(new Uint8[1]),
		size(0),
		data(),
		signals(0),
		id(new_id)
	{
		set_size(4);

		if ((signal1 >= 16)||(signal2 >= 16)) {
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Invalid network signal");
		}
		signals = signal1 << 4;
		signals += signal2 << 4 >> 4;
	}
	NetworkPacket::NetworkPacket(Uint8 new_id, Uint8 signal1, Uint8 signal2, size_t new_size, Uint8* new_data) :
		packet(new Uint8[1]),
		size(0),
		data(),
		signals(0),
		id(new_id)
	{
		set_size(4);

		if ((signal1 >= 16)||(signal2 >= 16)) {
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Invalid network signal");
		}
		signals = signal1 << 4;
		signals += signal2 << 4 >> 4;

		load_data(new_size, new_data);
	}
	NetworkPacket::NetworkPacket(UDPpacket* udp_data) :
		packet(new Uint8[1]),
		size(0),
		data(),
		signals(0),
		id(0)
	{
		if (udp_data->len < 4) {
			reset();
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Malformed UDP packet, the packet is now empty");
			return;
		}

		load_net(udp_data->data);
	}
	NetworkPacket::~NetworkPacket() {
		delete[] packet;
	}
	int NetworkPacket::reset() {
		size = 1;
		packet = new Uint8[1];
		packet[0] = 1;

		return 0;
	}

	int NetworkPacket::load_net(Uint8* new_data) {
		size_t new_size = new_data[0] << 8;
		new_size += new_data[1];
		set_size(new_size);

		id = new_data[2];
		signals = new_data[3];

		data.clear();
		data.reserve(size-4);
		for (size_t i=4; i<size; ++i) {
			data.push_back(new_data[i]);
		}

		return 0;
	}
	int NetworkPacket::load_data(size_t data_size, Uint8* new_data) {
		set_size(data_size+4);

		data.clear();
		data.reserve(data_size);
		for (size_t i=4; i<size; ++i) {
			data.push_back(new_data[i-4]);
		}

		return 0;
	}
	int NetworkPacket::append_data(size_t data_size, Uint8* new_data) {
		set_size(size+data_size);

		data.reserve(size-4);
		for (size_t i=0; i<data_size; ++i) {
			data.push_back(new_data[i]);
		}

		return 0;
	}
	int NetworkPacket::append_net(UDPpacket* udp_data) {
		Uint8* new_data = udp_data->data;

		size_t data_size = new_data[0] << 8;
		data_size += new_data[1];
		data_size -= 4;
		set_size(size+data_size);

		id = new_data[2];
		signals = new_data[3];

		data.reserve(size-4);
		for (size_t i=0; i<data_size; ++i) {
			data.push_back(new_data[i+4]);
		}

		return 0;
	}

	int NetworkPacket::set_size(size_t new_size) {
		delete[] packet;

		size = new_size;
		if (size > MAX_SIZE) {
			reset();
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Requested packet size is larger than MAX_SIZE: " + bee_itos(MAX_SIZE) + ", the packet is now empty");
			return 1;
		}

		packet = new Uint8[size];
		packet[0] = size >> 8;
		packet[1] = size;

		return 0;
	}

	Uint8* NetworkPacket::get() {
		if (data.size()+4 != size) {
			reset();
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to construct packet: invalid data size, the packet is now empty");
			return packet;
		}

		packet[2] = id;
		packet[3] = signals;

		for (size_t i=4; i<size; ++i) {
			packet[i] = data[i-4];
		}

		return packet;
	}
	std::pair<size_t,Uint8*> NetworkPacket::get_raw() {
		if (data.size()+4 != size) {
			reset();
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to construct packet: invalid data size, the packet is now empty");
			return std::make_pair(0, packet);
		}

		for (size_t i=0; i<size-4; ++i) {
			packet[i] = data[i-0];
		}
		for (size_t i=size-4; i<size; ++i) {
			packet[i] = 0;
		}

		return std::make_pair(size-4, packet);
	}

	size_t NetworkPacket::get_size() const {
		return size;
	}
	Uint8 NetworkPacket::get_signal1() const {
		return signals >> 4;
	}
	Uint8 NetworkPacket::get_signal2() const {
		return signals - (signals >> 4 << 4);
	}
}

#endif // BEE_NETWORK_PACKET
