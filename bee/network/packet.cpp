/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK_PACKET
#define BEE_NETWORK_PACKET 1

#include "packet.hpp"

#include "../util/platform.hpp"
#include "../util/networking.hpp"
#include "../util/real.hpp"

#include "../messenger/messenger.hpp"

#include "network.hpp"
#include "client.hpp"
#include "connection.hpp"
#include "data.hpp"

/*
	Network packet format {
		total message length,
		sequence data, // 10b for packet id, 3b for multipacket amount, 3b for multipacket index
		timestamp (in network time),
		sender id,
		signals,
		data
	}
*/

namespace bee {
	Uint16 NetworkPacket::next_id = 0;
	//const size_t NetworkPacket::MAX_SIZE = 65536;
	const size_t NetworkPacket::MAX_SIZE = 1024;
	const size_t NetworkPacket::META_SIZE = 14;

	NetworkPacket::NetworkPacket() :
		packet(),

		data(nullptr),
		multi_packet(),

		sequence(0),
		timestamp(0),

		id(0)
	{}
	NetworkPacket::NetworkPacket(Uint8 _id) :
		NetworkPacket()
	{
		id = _id;
	}
	NetworkPacket::NetworkPacket(Uint8 _id, NetworkData* _data) :
		NetworkPacket()
	{
		id = _id;
		data = _data;
	}
	NetworkPacket::NetworkPacket(Uint8 _id, E_NETSIG1 signal1, E_NETSIG2 signal2) :
		NetworkPacket()
	{
		id = _id;
		data = new NetworkData(signal1, signal2);
	}
	NetworkPacket::NetworkPacket(Uint8 _id, E_NETSIG1 signal1, E_NETSIG2 signal2, std::vector<Uint8> _data) :
		NetworkPacket()
	{
		id = _id;
		data = new NetworkData(signal1, signal2, _data);
	}
	NetworkPacket::NetworkPacket(UDPpacket* udp_data) :
		NetworkPacket()
	{
		if (static_cast<size_t>(udp_data->len) < META_SIZE) {
			reset();
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Malformed UDP packet, the packet is now empty");
			return;
		}

		load_net(udp_data);
	}
	NetworkPacket::NetworkPacket(const NetworkPacket& other) :
		packet(other.packet),

		data(new NetworkData(*other.data)),
		multi_packet(other.multi_packet),
		sequence(other.sequence),
		timestamp(other.timestamp),

		id(other.id)
	{}
	NetworkPacket::~NetworkPacket() {
		reset();
	}
	int NetworkPacket::reset() {
		if (data != nullptr) {
			delete data;
			data = nullptr;
		}

		packet.clear();

		free_multi();

		return 0;
	}
	int NetworkPacket::free_multi() {
		for (auto& p : multi_packet) {
			p.clear();
		}
		multi_packet.clear();
		return 0;
	}

	NetworkPacket& NetworkPacket::operator=(const NetworkPacket& rhs) {
		if (this != &rhs) {
			this->reset();
			this->packet = rhs.packet;

			this->data = new NetworkData(*rhs.data);
			this->multi_packet = rhs.multi_packet;
			this->sequence = rhs.sequence;
			this->timestamp = rhs.timestamp;

			this->id = rhs.id;
		}
		return *this;
	}

	int NetworkPacket::load_net(UDPpacket* udp_data) {
		Uint8* new_data = udp_data->data;

		sequence = new_data[6] << 8;
		sequence += new_data[7];

		timestamp = new_data[8] << 24;
		timestamp += new_data[9] << 16;
		timestamp += new_data[10] << 8;
		timestamp += new_data[11];

		id = new_data[12];

		data = new NetworkData(new_data[13]);

		append_net(udp_data);

		return 0;
	}
	int NetworkPacket::append_net(UDPpacket* udp_data) {
		Uint8* new_data = udp_data->data;

		size_t data_size = new_data[4] << 8;
		data_size += new_data[5];

		std::vector<Uint8> p;
		p.reserve(data_size);
		for (size_t i=0; i<data_size; ++i) {
			p.push_back(new_data[i]);
		}

		if (!verify_data(p)) {
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to append net data: packet checksum failed");
			return 1;
		}

		multi_packet.push_back(p);

		if (get_is_sequence_complete()) {
			get_data();
		}

		return 0;
	}
	int NetworkPacket::load_data(NetworkData* _data) {
		data = _data;
		return 0;
	}
	int NetworkPacket::append_data(NetworkData* _data) {
		data->append_data(_data->get());
		return 0;
	}

	bool NetworkPacket::get_is_sequence_complete() {
		Uint8 amount = (sequence - (sequence >> 6 << 6)) >> 3;
		if (multi_packet.size() == amount) {
			return true;
		}
		return false;
	}

	Uint16 NetworkPacket::get_next_id() {
		int id = next_id++;
		if (next_id >= 1024) {
			next_id = 0;
		}
		return id;
	}
	bool NetworkPacket::verify_data(std::vector<Uint8>& data) {
		Uint32 checksum = data[0] << 24;
		checksum += data[1] << 16;
		checksum += data[2] << 8;
		checksum += data[3];

		data[0] = -1;
		data[1] = -1;
		data[2] = -1;
		data[3] = -1;

		bool r = verify_checksum(data, checksum);

		data[0] = checksum >> 24;
		data[1] = checksum >> 16;
		data[2] = checksum >> 8;
		data[3] = checksum;

		return r;
	}

	std::vector<Uint8> NetworkPacket::get() {
		std::vector<Uint8> d = data->get();
		size_t size = META_SIZE + d.size();

		packet.clear();
		packet.resize(META_SIZE);

		packet[0] = -1;
		packet[1] = -1;
		packet[2] = -1;
		packet[3] = -1;

		packet[4] = static_cast<Uint8>(size >> 8);
		packet[5] = static_cast<Uint8>(size);

		sequence = get_next_id() << 6;
		sequence += 1 << 3;
		packet[6] = sequence >> 8;
		packet[7] = static_cast<Uint8>(sequence);

		timestamp = net::get_time();
		packet[8] = timestamp >> 24;
		packet[9] = timestamp >> 16;
		packet[10] = timestamp >> 8;
		packet[11] = timestamp;

		packet[12] = id;
		packet[13] = data->get_signals();

		for (auto& e : d) {
			packet.push_back(e);
		}

		Uint32 checksum = get_checksum(packet);
		packet[0] = checksum >> 24;
		packet[1] = checksum >> 16;
		packet[2] = checksum >> 8;
		packet[3] = checksum;

		return packet;
	}
	const std::vector<std::vector<Uint8>>& NetworkPacket::get_multi() {
		free_multi();

		std::vector<Uint8> d = data->get();

		size_t size = d.size() + META_SIZE;
		int amount = size/MAX_SIZE;
		if (size % MAX_SIZE != 0) {
			++amount;
		}
		sequence = get_next_id() << 6;
		sequence += amount << 3;

		Uint8 sig = data->get_signals();

		timestamp = net::get_time();

		size_t remaining_size = size;
		while (remaining_size > 0) {
			size_t s = remaining_size;
			if (s > MAX_SIZE) {
				s = MAX_SIZE;
			}
			remaining_size -= s;

			std::vector<Uint8> p;
			p.resize(META_SIZE);

			p[0] = -1;
			p[1] = -1;
			p[2] = -1;
			p[3] = -1;

			p[4] = static_cast<Uint8>(s >> 8);
			p[5] = static_cast<Uint8>(s);

			p[6] = sequence >> 8;
			p[7] = static_cast<Uint8>(sequence + multi_packet.size());

			p[8] = timestamp >> 24;
			p[9] = timestamp >> 16;
			p[10] = timestamp >> 8;
			p[11] = timestamp;

			p[12] = id;
			p[13] = sig;

			for (size_t i=META_SIZE; i<s; ++i) {
				p.push_back(d[i-META_SIZE + multi_packet.size()*MAX_SIZE]);
			}

			Uint32 checksum = get_checksum(p);
			p[0] = checksum >> 24;
			p[1] = checksum >> 16;
			p[2] = checksum >> 8;
			p[3] = checksum;

			multi_packet.push_back(p);
		}

		return multi_packet;
	}
	NetworkData* NetworkPacket::get_data() {
		if (!get_is_sequence_complete()) {
			messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to construct net data: incomplete sequence");
			return nullptr;
		}

		Uint8 sig = 0;
		if (data != nullptr) {
			sig = data->get_signals();
			delete data;
			data = nullptr;
		}
		data = new NetworkData(sig);

		Uint8 amount = (sequence - (sequence >> 6 << 6)) >> 3;
		for (size_t i=0; i<amount; ++i) {
			bool has_found_data = false;
			for (auto& p : multi_packet) {
				Uint8 pid = p[7] - (p[7] >> 3 << 3);
				if (pid == i) {
					data->append_data(p, META_SIZE); // Append only the raw data

					has_found_data = true;
					break;
				}
			}

			if (!has_found_data) {
				messenger::send({"engine", "network"}, E_MESSAGE::ERROR, "Failed to assemble packet sequence: missing piece " + bee_itos(i) + " of " + bee_itos(amount));
				return nullptr;
			}
		}

		return data;
	}
	std::vector<Uint8> NetworkPacket::get_raw() const {
		if (data == nullptr) {
			return std::vector<Uint8>();
		}

		return data->get();
	}

	Uint16 NetworkPacket::get_packet_id() const {
		return sequence >> 6;
	}
	Uint32 NetworkPacket::get_time() const {
		return timestamp;
	}

	E_NETSIG1 NetworkPacket::get_signal1() const {
		if (data == nullptr) {
			return E_NETSIG1::INVALID;
		}

		return data->get_signal1();
	}
	E_NETSIG2 NetworkPacket::get_signal2() const {
		if (data == nullptr) {
			return E_NETSIG2::INVALID;
		}

		return data->get_signal2();
	}
	size_t NetworkPacket::get_size() const {
		if (data == nullptr) {
			return -1;
		}

		return data->get().size() + META_SIZE;
	}

namespace net {
	/*
	* send_packet() - Send the given packet to the given client
	*/
	int send_packet(const NetworkClient& client, std::unique_ptr<NetworkPacket> const & packet) {
		if (packet->get_size() > NetworkPacket::MAX_SIZE) {
			const std::vector<std::vector<Uint8>>& packets = packet->get_multi();
			for (auto& p : packets) {
				network_udp_send(client.sock, client.channel, p.size(), p.data());
			}
			int r = packets.size();
			packet->free_multi();
			return r;
		}

		return network_udp_send(client.sock, client.channel, packet->get_size(), packet->get().data());
	}
	/*
	* recv_packet() - Attempt to receive a packet from the UDP socket
	*/
	std::unique_ptr<NetworkPacket> recv_packet(NetworkConnection* connection) {
		if (connection == nullptr) {
			return nullptr;
		}

		connection->udp_data = network_packet_realloc(connection->udp_data, NetworkPacket::MAX_SIZE); // Attempt to allocate more space to receive data
		if (connection->udp_data == nullptr) {
			return nullptr; // Return nullptr when failed to allocate
		}

		int r = network_udp_recv(connection->udp_sock, connection->udp_data); // Attempt to receive data over the UDP socket
		if (r == 0) {
			return nullptr; // Return nullptr when there is no message to receive
		}

		if (r == -1) { // If receiving failed, attempt to allocate more space for the packet
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Failed to receive packet");
			return nullptr; // Return nullptr when the message still could not be received
		}

		return std::make_unique<NetworkPacket>(
			connection->udp_data
		);
	}
}}

#endif // BEE_NETWORK_PACKET
