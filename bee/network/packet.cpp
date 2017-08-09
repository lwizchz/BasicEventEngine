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
#include "../util/network.hpp"

#include "../messenger/messenger.hpp"

#include "client.hpp"
#include "connection.hpp"

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
		free_multi();

		size = 1;
		packet = new Uint8[1];
		packet[0] = 1;

		return 0;
	}
	int NetworkPacket::free_multi() {
		for (auto& p : multi_packet) {
			delete[] p.second;
		}
		multi_packet.clear();
		return 0;
	}

	int NetworkPacket::load_net(Uint8* new_data) {
		size_t new_size = new_data[0] << 8;
		new_size += new_data[1];
		if (set_size(new_size)) {
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to load net data: allocation error");
			return 1;
		}

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
		if (set_size(data_size+4)) {
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to append net data: allocation error");
			return 1;
		}

		data.clear();
		data.reserve(data_size);
		for (size_t i=4; i<size; ++i) {
			data.push_back(new_data[i-4]);
		}

		return 0;
	}
	int NetworkPacket::load_data(std::pair<size_t,Uint8*> new_data) {
		return load_data(new_data.first, new_data.second);
	}
	int NetworkPacket::append_data(size_t data_size, Uint8* new_data) {
		if (set_size(size+data_size)) {
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to append data: allocation error");
			return 1;
		}

		data.reserve(size-4);
		for (size_t i=0; i<data_size; ++i) {
			data.push_back(new_data[i]);
		}

		delete[] new_data;

		return 0;
	}
	int NetworkPacket::append_data(std::pair<size_t,Uint8*> new_data) {
		return append_data(new_data.first, new_data.second);
	}
	int NetworkPacket::append_net(UDPpacket* udp_data) {
		Uint8* new_data = udp_data->data;

		size_t data_size = new_data[0] << 8;
		data_size += new_data[1];
		data_size -= 4;
		if (set_size(size+data_size)) {
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to append net data: allocation error");
			return 1;
		}

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
	const std::vector<std::pair<size_t,Uint8*>>& NetworkPacket::get_multi() {
		free_multi();

		if (data.size()+4 != size) {
			reset();
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to construct packet: invalid data size, the packet is now empty");
			return multi_packet;
		}

		Uint8 sig = get_signal1() << 4;
		sig += 3 << 4 >> 4;

		size_t remaining_size = size;
		while (remaining_size > 0) {
			size_t s = remaining_size;
			if (s > MAX_SIZE) {
				s = MAX_SIZE;
				remaining_size -= MAX_SIZE;
			}

			std::pair<size_t,Uint8*> p (s, new Uint8[s]);
			p.second[0] = size >> 8;
			p.second[1] = size;

			p.second[2] = id;
			p.second[3] = sig;

			for (size_t i=4; i<s; ++i) {
				p.second[i] = data[i-4 + multi_packet.size()*MAX_SIZE];
			}

			multi_packet.push_back(p);
		}

		return multi_packet;
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

	/*
	* send_packet() - Send the given packet to the given client
	*/
	int send_packet(const NetworkClient& client, std::unique_ptr<NetworkPacket> const & packet) {
		if ((packet->get_signal2() == 4)&&(packet->get_size() > NetworkPacket::MAX_SIZE)) {
			const std::vector<std::pair<size_t,Uint8*>>& packets = packet->get_multi();
			for (auto& p : packets) {
				network_udp_send(client.sock, client.channel, p.first, p.second);
			}
			packet->free_multi();
			return packets.size();
		}

		return network_udp_send(client.sock, client.channel, packet->get_size(), packet->get());
	}
	/*
	* recv_packet() - Attempt to receive a packet from the UDP socket
	*/
	std::unique_ptr<NetworkPacket> recv_packet(NetworkConnection* connection) {
		if (connection == nullptr) {
			return nullptr;
		}

		//connection->udp_data = network_packet_realloc(connection->udp_data, 512); // Attempt to allocate space to receive data
		connection->udp_data = network_packet_realloc(connection->udp_data, 65536); // Attempt to allocate more space to receive data
		if (connection->udp_data == nullptr) {
			return nullptr; // Return nullptr when failed to allocate
		}

		int r = network_udp_recv(connection->udp_sock, connection->udp_data); // Attempt to receive data over the UDP socket
		if (r == 0) {
			return nullptr; // Return nullptr when there is no message to receive
		}

		if (r == -1) { // If receiving failed, attempt to allocate more space for the packet
			/*connection->udp_data = network_packet_realloc(connection->udp_data, 65536); // Attempt to allocate more space to receive data
			if (connection->udp_data == nullptr) {
				return nullptr; // Return nullptr when failed to allocate
			}

			r = network_udp_recv(connection->udp_sock, connection->udp_data); // Attempt to receive data over the UDP socket*/
			if (r != 1) {
				return nullptr; // Return nullptr when the message still could not be received
			}
		}

		return std::make_unique<NetworkPacket>(
			connection->udp_data
		);
	}
}

#endif // BEE_NETWORK_PACKET
