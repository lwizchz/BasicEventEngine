/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_NETWORK
#define BEE_NETWORK 1

#include <SDL2/SDL_net.h>

#include "network.hpp"

#include "../debug.hpp"
#include "../engine.hpp"

#include "../util/real.hpp"
#include "../util/string.hpp"
#include "../util/network.hpp"
#include "../util/template.hpp"
#include "../util/platform.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/instance.hpp"
#include "../core/enginestate.hpp"
#include "../core/room.hpp"

#include "connection.hpp"
#include "event.hpp"

#include "../resource/room.hpp"

/*
	Network message format {
		total message length,
		sender id,
		signals,
		data
	}
*/

/*
	Network signals (half a byte each)
	1	connect requested/accepted
	2	disconnect notification
	3	server info request
		0	keep alive
		1	server name
		2	player map
		3	data update
		4	data commit
	4	client info update
		3	data update
		4	data commit
*/

namespace bee { namespace net {
	namespace internal {
		bool is_initialized = false;

		int port = 3054;
		Uint32 timeout = 1000;

		std::map<std::string,std::string> servers;

		NetworkConnection* connection = nullptr;
		bool has_data_update = false;
	}

	/*
	* init() - Initialize the networking utilities
	*/
	int init() {
		if (!get_options().is_network_enabled) {
			return 1; // Return 1 if networking is disabled
		}

		if (network_init()) { // Attempt to initialize networking
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Error initializing network functionality: " + get_sdl_error());
			return 2; // Return 2 on failure to init
		}

		internal::is_initialized = true; // Mark the network as ready

		return 0; // Return 0 on success
	}
	/*
	* get_is_initialized() - Return whether networking has been initialized or not
	*/
	bool get_is_initialized() {
		return internal::is_initialized;
	}
	/*
	* close() - Close and clean up the networking utilities
	*/
	int close() {
		if (!get_options().is_network_enabled) {
			return 1; // Return 1 if networking is disabled
		}

		if (!internal::is_initialized) {
			return 0; // Return 0 if there is nothing to close
		}

		session_end();

		network_close(); // Close networking
		internal::is_initialized = false; // Mark the network as not ready

		return 0; // Return 0 on success
	}

	/*
	* send_packet() - Send the given packet to the given client
	*/
	int send_packet(const NetworkClient& client, std::unique_ptr<NetworkPacket> const &packet) {
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
	std::unique_ptr<NetworkPacket> recv_packet() {
		if (internal::connection == nullptr) {
			return nullptr;
		}

		//internal::connection->udp_data = network_packet_realloc(internal::connection->udp_data, 512); // Attempt to allocate space to receive data
		internal::connection->udp_data = network_packet_realloc(internal::connection->udp_data, 65536); // Attempt to allocate more space to receive data
		if (internal::connection->udp_data == nullptr) {
			return nullptr; // Return nullptr when failed to allocate
		}

		int r = network_udp_recv(internal::connection->udp_sock, internal::connection->udp_data); // Attempt to receive data over the UDP socket
		if (r == 0) {
			return nullptr; // Return nullptr when there is no message to receive
		}

		if (r == -1) { // If receiving failed, attempt to allocate more space for the packet
			/*internal::connection->udp_data = network_packet_realloc(internal::connection->udp_data, 65536); // Attempt to allocate more space to receive data
			if (internal::connection->udp_data == nullptr) {
				return nullptr; // Return nullptr when failed to allocate
			}

			r = network_udp_recv(internal::connection->udp_sock, internal::connection->udp_data); // Attempt to receive data over the UDP socket*/
			if (r != 1) {
				return nullptr; // Return nullptr when the message still could not be received
			}
		}

		return std::unique_ptr<NetworkPacket>(new NetworkPacket(
			internal::connection->udp_data
		));
	}

	/*
	* handle_events() - Handle network sending and receiving during the event loop
	*/
	int handle_events() {
		if (
			(!get_options().is_network_enabled)
			||(!internal::is_initialized)
			||(internal::connection == nullptr)
		) {
			return 1; // Return 1 if networking is disabled or if it has not been initialized
		}

		auto packet = recv_packet();
		while (packet != nullptr) { // Iterate over the received packets
			if (internal::connection->udp_data->len < 3) {
				packet = recv_packet();
				continue; // Continue when the data doesn't contain a signal
			}

			if (internal::connection->is_host) { // Handle session hosting signals
				switch (packet->get_signal1()) {
					case 1: { // Connection requested
						if (internal::connection->players.size() < internal::connection->max_players) { // If there is still room for clients then allow the current one to connect
							NetworkClient c;
							c.id = internal::connection->players.size(); // Get an id for the client

							int port = -1;
							std::tie(port, c.sock) = network_udp_open_range(internal::port, internal::connection->max_players);
							if (c.sock == nullptr) {
								messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not accept client");
								break;
							}

							IPaddress ipa = internal::connection->udp_data->address;
							//ipa.port = port;
							c.channel = network_udp_bind(&c.sock, c.id, &ipa); // Bind a sending socket to the client who is requesting a connection
							if (c.channel == -1) {
								network_udp_close(&c.sock);

								messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not accept client");
								break;
							}

							c.name = chra(packet->get_raw());

							Uint8* data = new Uint8[1];
							data[0] = c.id;
							auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
								internal::connection->self_id,
								1, // Connection request signal
								0,
								1, // Data size
								data
							));

							if (send_packet(c, p) == 0) { // Send the client their id
								network_udp_close(&c.sock);
								delete[] data;

								messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not accept client");
								break;
							}
							c.last_recv = get_ticks();

							internal::connection->players.emplace(c.id, c); // Add the client to the list of clients

							delete[] data;

							NetworkEvent e (E_NETEVENT::CONNECT);
							e.data.emplace("id", c.id);
							e.data.emplace("username", c.name);
							get_current_room()->network(e);

							messenger::send({"engine", "network"}, E_MESSAGE::INFO, "Client accepted");

							// Send player update to clients
							internal::host_send_players(-1);
						} else {
							NetworkClient c;

							int port = -1;
							std::tie(port, c.sock) = network_udp_open_range(internal::port, internal::connection->max_players);
							if (c.sock == nullptr) {
								messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not accept client");
								break;
							}

							auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
								internal::connection->self_id,
								2, // Disconnect signal
								0
							));
							p->append_data(orda("Server full"));

							send_packet(c, p);

							network_udp_close(&c.sock);

							messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not accept client: server full");
						}
						break;
					}
					case 2: { // Client disconnected
						NetworkClient& c = internal::connection->players[packet->id];
						network_udp_unbind(&c.sock, c.channel); // Unbind the socket to the client who has disconnected
						network_udp_close(&c.sock); // Close the socket

						internal::connection->players.erase(c.id); // Remove them from the list of clients

						NetworkEvent e (E_NETEVENT::DISCONNECT);
						e.data.emplace("id", c.id);
						e.data.emplace("username", c.name);
						get_current_room()->network(e);

						messenger::send({"engine", "network"}, E_MESSAGE::INFO, "Client disconnected");

						// Send player update to clients
						internal::host_send_players(-1);

						break;
					}
					case 3: { // Server info request
						switch (packet->get_signal2()) {
							case 0: {
								NetworkClient& c = internal::connection->players[packet->id];
								c.last_recv = get_ticks();
								break;
							}
							case 1: { // Client requesting server name
								NetworkClient c;
								std::tie(std::ignore, c.sock) = network_udp_open_range(internal::port, internal::connection->max_players+1);
								if (c.sock == nullptr) {
									messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not open socket");
									break;
								}

								std::pair<size_t,Uint8*> name = orda(internal::connection->name); // Convert the server name to Uint8's

								// See Network Message Format at the top of this file for details
								auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
									internal::connection->self_id,
									3, // Info request signal
									1, // Info type 1: server name
									name.first, // Data size
									name.second // Data
								));

								if (send_packet(c, p) == 0) { // Send the entire message
									network_udp_close(&c.sock);
									free(name.second);

									messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not send server info");
									break;
								}

								network_udp_close(&c.sock);

								free(name.second); // Free the allocated space

								break;
							}
							case 2: { // Client requesting player map
								internal::host_send_players(packet->id);
								break;
							}
							case 3: { // Client requesting data map
								internal::host_send_data(packet->id);
								break;
							}
							default: {
								messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Unknown network signal: 3." + bee_itos(packet->get_signal2()));
							}
						}
						break; // Break from signal1: 3, server info request
					}
					case 4: { // Client info update
						switch (packet->get_signal2()) {
							case 3:
							case 4: {
								// Buffer the received data if necessary
								if (internal::connection->tmp_data_buffer == nullptr) {
									internal::connection->tmp_data_buffer = std::unique_ptr<NetworkPacket>(new NetworkPacket(
										internal::connection->udp_data
									));
								} else {
									internal::connection->tmp_data_buffer->append_net(internal::connection->udp_data);
								}

								if (internal::connection->tmp_data_buffer->get_signal2() == 3) { // Break when only a partial map has been received
									break;
								}

								std::map<std::string,SIDP> new_data;
								network_map_decode(internal::connection->tmp_data_buffer->get_raw(), &new_data); // Decode the data map into a temporary map
								internal::connection->data.insert(new_data.begin(), new_data.end()); // Insert the new data into the connection data map

								internal::connection->tmp_data_buffer.reset();
								internal::connection->tmp_data_buffer = nullptr;

								internal::has_data_update = true;

								NetworkEvent e (E_NETEVENT::DATA_UPDATE);
								e.data = std::map<std::string,SIDP>(internal::connection->data);
								get_current_room()->network(e);

								break;
							}
							default: {
								messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Unknown network signal: 3." + bee_itos(packet->get_signal2()));
							}
						}
						break; // Break from signal1: 4, client info update
					}
					default: {
						messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Unknown network signal: " + bee_itos(packet->get_signal1()) + ".x");
					}
				}
			} else { // Handle client signals
				internal::connection->last_recv = get_ticks();
				switch (packet->get_signal1()) {
					case 1: { // Connection accepted
						internal::connection->self_id = packet->get_raw().second[0]; // Read the id that the server assigned to us
						internal::connection->is_connected = true; // Mark our networking as connected

						NetworkEvent e (E_NETEVENT::CONNECT);
						e.data.emplace("id", internal::connection->self_id);
						get_current_room()->network(e);

						// Request an initial player map
						/*NetworkClient c (internal::connection->udp_sock, internal::connection->channel);
						auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
							internal::connection->self_id,
							3, // Info request signal
							2 // Info type 2: player map
						));
						if (send_packet(c, p) == 0) { // Send the keep alive
							messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not request player map from server");
							break;
						}*/

						messenger::send({"engine", "network"}, E_MESSAGE::INFO, "Connected to server with id " + bee_itos(internal::connection->self_id));
						break;
					}
					case 2: { // Disconnected by host
						session_end(); // Reset session

						NetworkEvent e (E_NETEVENT::DISCONNECT);
						e.data.emplace("reason", chra(packet->get_raw()));
						get_current_room()->network(e);

						messenger::send({"engine", "network"}, E_MESSAGE::INFO, "Disconnected by server");
						return 2; // Return 2 when disconnected in the middle of the event loop
					}
					case 3: { // Server info received
						switch (packet->get_signal2()) {
							case 0: {
								NetworkClient c (internal::connection->udp_sock, internal::connection->channel);

								// See Network Message Format at the top of this file for details
								auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
									internal::connection->self_id,
									3, // Info request signal
									0 // Info type 0: keepalive
								));
								if (send_packet(c, p) == 0) { // Send the keep alive
									messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not send keep alive to server");
									break;
								}

								break;
							}
							case 1: { // Received server name (along with IP address)
								std::string ip = network_get_address(internal::connection->udp_data->address.host); // Get the server IP
								std::string name = chra(packet->get_raw()); // Get the server name from the data

								internal::servers.emplace(ip, name); // Add the server to the list of available servers

								break;
							}
							case 2: { // Received player map
								std::map <std::string,std::string> t; // Temp map to store player names in
								internal::connection->players.clear(); // Clear out old player data
								NetworkEvent e (E_NETEVENT::PLAYER_UPDATE);

								network_map_decode(packet->get_raw(), &t); // Decode the player map from the data into t
								for (auto& p : t) { // Iterate over the players in the temp map in order to generate sockets for each of them
									NetworkClient c;
									c.name = p.second;

									internal::connection->players.emplace(bee_stoi(p.first), c); // Insert the player's id and IP address into our copy of the player map
									e.data.emplace(p.first, p.second);
								}

								get_current_room()->network(e);

								break;
							}
							case 3: // Received partial data map
							case 4: { // Received end of data map
								// Buffer the received data if necessary
								if (internal::connection->tmp_data_buffer == nullptr) {
									internal::connection->tmp_data_buffer = std::unique_ptr<NetworkPacket>(new NetworkPacket(
										internal::connection->udp_data
									));
								} else {
									internal::connection->tmp_data_buffer->append_net(internal::connection->udp_data);
								}

								if (internal::connection->tmp_data_buffer->get_signal2() == 3) { // Break when only a partial map has been received
									break;
								}

								network_map_decode(internal::connection->tmp_data_buffer->get_raw(), &internal::connection->data); // Decode the data map directly into net->data in order to immediately replace all old data

								internal::connection->tmp_data_buffer.reset();
								internal::connection->tmp_data_buffer = nullptr;

								NetworkEvent e (E_NETEVENT::DATA_UPDATE);
								e.data = std::map<std::string,SIDP>(internal::connection->data);
								get_current_room()->network(e);

								break;
							}
							default: {
								messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Unknown network signal: 3." + bee_itos(packet->get_signal2()));
							}
						}
						break; // Break from signal1: 3 info received
					}
					default: {
						messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Unknown network signal: " + bee_itos(packet->get_signal1()) + ".x");
					}
				}
			}

			packet = recv_packet();
		}

		if (internal::has_data_update) {
			if (internal::connection->is_host) {
				internal::host_send_data(-1);
			} else {
				std::pair<size_t,Uint8*> data_map = network_map_encode(internal::connection->data); // Encode the data map as a series of Uint8's

				// See Network Message Format at the top of this file for details
				auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
					internal::connection->self_id,
					4, // Info update signal
					4, // Info type 3: data map
					data_map.first, // Data size
					data_map.second // Data
				));

				NetworkClient c (internal::connection->udp_sock, internal::connection->channel);

				if (send_packet(c, p) == 0) { // Send the entire message
					messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not send data map to server");
				}

				free(data_map.second); // Free the allocated space
			}
		}

		const Uint32 now = get_ticks();
		if (internal::connection->is_host) { // If we are the host
			auto p_disconnect = std::unique_ptr<NetworkPacket>(new NetworkPacket(
				internal::connection->self_id,
				2, // Disconnect signal
				0
			));
			p_disconnect->append_data(orda("Timed out after " + bee_itos(internal::timeout) + "ms"));
			auto p_keepalive = std::unique_ptr<NetworkPacket>(new NetworkPacket(
				internal::connection->self_id,
				3, // Keepalive signal
				0
			));

			for (auto& player : internal::connection->players) { // Iterate over the clients to ensure they are still connected
				if (player.first != 0) { // Don't disconnect ourselves
					if (now - player.second.last_recv > internal::timeout) { // Only disconnect clients that have timed out
						send_packet(player.second, p_disconnect); // Send a disconnection signal
						network_udp_unbind(&player.second.sock, player.second.channel); // Unbind the socket
						network_udp_close(&player.second.sock); // Close the socket

						internal::connection->players.erase(player.first); // Remove the client from the map
						messenger::send({"engine", "network"}, E_MESSAGE::INFO, "Client timed out");
					} else if (now - player.second.last_recv > internal::timeout/2) { // Send a keep alive to clients that might be timing out
						if (send_packet(player.second, p_keepalive) == 0) { // Send the keep alive
							messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not send keep alive to client");
						}
					}
				}
			}
		} else {
			if (now - internal::connection->last_recv > internal::timeout) {
				session_end();
			} else if (now - internal::connection->last_recv > internal::timeout/2) {
				// Wait for server to send keep alive?
			}
		}

		return 0; // Return 0 on success
	}

	/*
	* session_start() - Begin hosting a network session for others to join over UDP
	* @session_name: the name that the session will identify as
	* @max_players: the maximum amount of people connected to the session including the host
	* @player_name: the name of the player who's hosting
	*/
	int session_start(const std::string& session_name, int max_players, const std::string& player_name) {
		// Reset the session
		session_end();
		internal::connection = new NetworkConnection();

		internal::connection->udp_sock = network_udp_open(internal::port); // Open a UDP listening socket to receive from all clients
		if (internal::connection->udp_sock == nullptr) {
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Failed to start new session \"" + session_name + "\"");
			return 1; // Return 1 if the socket failed to open
		}

		messenger::send({"engine", "network"}, E_MESSAGE::INFO, "Started new session \"" + session_name + "\" with " + bee_itos(max_players) + " max players");

		// Set the session info
		internal::connection->name = session_name;
		internal::connection->max_players = max_players;
		internal::connection->self_id = 0;
		internal::connection->players.emplace(internal::connection->self_id, NetworkClient()); // Insert ourself into the player map

		// Set connection info
		internal::connection->is_connected = true;
		internal::connection->is_host = true;

		return 0; // Return 0 on success
	}
	/*
	* session_find() - Query the local network for available sessions
	*/
	const std::map<std::string,std::string>& session_find() {
		internal::servers.clear(); // Clear the previously available servers

		if ((internal::connection != nullptr)&&(internal::connection->is_connected)) {
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "net::session_find() : Failed to find sessions, already connected");
			return internal::servers; // Return 1 if we are already connected
		}

		// Reset the session
		session_end();
		internal::connection = new NetworkConnection();

		internal::connection->udp_sock = network_udp_open(internal::port);
		internal::connection->channel = network_udp_bind(&internal::connection->udp_sock, 0, "192.168.1.255", internal::port); // Bind a sending socket to the broadcast IP 192.168.1.255
		if (internal::connection->channel == -1) {
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "net::session_find() : UDP broadcast socket failed to bind");
			return internal::servers; // Return an empty map if the sending socket failed to bind
		}

		NetworkClient c (internal::connection->udp_sock, internal::connection->channel);
		auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
			internal::connection->self_id,
			3, // Info request signal
			1 // Info type 1: server name
		));

		if (send_packet(c, p) == 0) { // Send a server name info request
			network_udp_close(&internal::connection->udp_sock);

			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "net::session_find() : Failed to send request on port " + bee_itos(internal::port));
			return internal::servers; // Return an empty map if the message failed to send
		}
		p.reset();

		internal::connection->udp_data = network_packet_realloc(internal::connection->udp_data, 256);
		if (internal::connection->udp_data == nullptr) { // Attempt to allocate space to receive data
			network_udp_close(&internal::connection->udp_sock); // Close the socket

			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "net::session_find() : Failed to allocate space to receive data");
			return internal::servers; // Return an empty map if the allocation failed
		}

		Uint32 t = get_ticks(); // Get the current time
		int r = network_udp_recv(internal::connection->udp_sock, internal::connection->udp_data); // Attempt to receive data
		while (get_ticks() - t < internal::timeout) { // Continue receiving until we timeout
			if ((r == 1)&&(internal::connection->udp_data->len > 4)) {
				p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
					internal::connection->udp_data
				));

				if (
					(p->get_size() > 4)
					&&(p->get_signal1() == 3)
					&&(p->get_signal2() == 1)
				) {
					std::string name = chra(p->get_raw()); // Get the server name from the data
					internal::servers.emplace(network_get_address(internal::connection->udp_data->address.host), name); // Add the server to the list of available servers
				}
			}

			r = network_udp_recv(internal::connection->udp_sock, internal::connection->udp_data); // Attempt to receive more data
		}

		network_udp_close(&internal::connection->udp_sock); // Close the socket
		return internal::servers; // Return the filled map on success
	}
	/*
	* session_join() - Attempt to join a session at the given IP address
	* @ip: the IP address to attempt to connect to
	* @player_name: the name of the player which will be sent to the host
	*/
	int session_join(const std::string& ip, const std::string& player_name) {
		if ((internal::connection != nullptr)&&(internal::connection->is_connected)) {
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "net::session_join() : Failed to join session, already connected");
			return 1; // Return 1 if we are already connected
		}

		// Reset session
		session_end();
		internal::connection = new NetworkConnection();

		internal::connection->players.clear(); // Clear the previous player map

		internal::connection->udp_sock = network_udp_open(internal::port);
		internal::connection->channel = network_udp_bind(&internal::connection->udp_sock, 0, ip, internal::port); // Bind a sending socket to the given server IP address
		if (internal::connection->channel == -1) {
			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "net::session_join() : Failed to bind to " + ip + " on port " + bee_itos(internal::port));
			return 2; // Return 2 if the socket failed to bind
		}

		NetworkClient c (internal::connection->udp_sock, internal::connection->channel);
		auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
			0,
			1,
			0
		));
		p->append_data(orda(player_name));

		if (send_packet(c, p) == 0) { // Send a server connection request
			network_udp_close(&internal::connection->udp_sock); // Close the socket

			messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "net::session_join() : Failed to send request to " + ip + " on port " + bee_itos(internal::port));
			return 3; // Return 3 if the messsage failed to send
		}

		internal::connection->last_recv = get_ticks();

		return 0; // Return 0 on success
	}
	/*
	* get_is_connected() - Return whether the network is currently connected to a session
	*/
	bool get_is_connected() {
		if (internal::connection == nullptr) {
			return false;
		}
		return internal::connection->is_connected;
	}
	/*
	* session_end() - End the session connection
	*/
	int session_end() {
		if (internal::connection == nullptr) {
			return 1; // Return 1 when nothing needs to be ended
		}

		if (internal::connection->is_connected) {
			if (internal::connection->is_host) { // If we are the host
				messenger::send({"engine", "network"}, E_MESSAGE::INFO, "Disconnecting " + bee_itos(internal::connection->players.size()-1) + " clients...");

				auto packet = std::unique_ptr<NetworkPacket>(new NetworkPacket(
					internal::connection->self_id,
					2, // Disconnect signal
					0
				));
				packet->append_data(orda("Server shutting down"));

				for (auto& player : internal::connection->players) { // Iterate over the clients to disconnect them
					if (player.first != 0) { // Only disconnect clients that aren't us
						send_packet(player.second, packet); // Send a disconnection signal
						network_udp_unbind(&player.second.sock, player.second.channel); // Unbind the socket
						network_udp_close(&player.second.sock); // Close the socket
					}
				}

				// Reset connection info
				internal::connection->is_host = false;
			} else {
				NetworkClient c (internal::connection->udp_sock, internal::connection->channel);
				auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
					internal::connection->self_id,
					2, // Disconnect signal
					0
				));
				send_packet(c, p); // Send a disconnection signal to the server
			}
		}

		// Close socket and free data
		if (internal::connection->udp_sock != nullptr) {
			network_udp_close(&internal::connection->udp_sock);
			internal::connection->udp_sock = nullptr;
		}
		if (internal::connection->udp_data != nullptr) {
			network_packet_free(internal::connection->udp_data);
			internal::connection->udp_data = nullptr;
		}

		// Reset connection and client info
		internal::connection->is_connected = false;
		internal::connection->name.clear();
		internal::connection->players.clear();
		internal::connection->data.clear();
		internal::servers.clear();
		internal::has_data_update = false;

		delete internal::connection;
		internal::connection = nullptr;

		return 0; // Return 0 on success
	}

	/*
	* session_sync_data() - Sync the given data to the session
	* @key: the data name
	* @value: the data itself
	*/
	int session_sync_data(const std::string& key, const std::string& value) {
		if (internal::connection == nullptr) {
			return 1; // Return 1 when there is no session
		}

		internal::connection->data[key] = value;
		internal::has_data_update = true;

		return 0;
	}
	/*
	* session_sync_instance() - Sync the given instance to the session
	* @inst: the instance to sync
	*/
	int session_sync_instance(Instance* inst) {
		if (internal::connection == nullptr) {
			return 1; // Return 1 when there is no session
		}

		internal::connection->data["inst_"+bee_itos(inst->id)] = inst->serialize();
		internal::has_data_update = true;

		return 0;
	}
	/*
	* session_sync_player() - Sync the given player instance
	* @id: the id of the player to sync
	* @inst: the instance controlled by the player
	*/
	int session_sync_player(int id, Instance* inst) {
		if (internal::connection == nullptr) {
			return 1; // Return 1 when there is no session
		}

		internal::connection->data["player_"+bee_itos(id)] = inst->serialize();
		internal::has_data_update = true;

		return 0;
	}

	/*
	* get_print() - Return an info string about the network session
	*/
	std::string get_print() {
		std::stringstream s;
		s << "Network status: ";

		if ((internal::connection != nullptr)&&(internal::connection->is_connected)) {
			if (internal::connection->is_host) {
				s << "host";
			} else {
				s << "client";
			}
		} else {
			s << "unconnected";
			return s.str();
		}

		s << "\n" << internal::connection->players.size()-1 << " players connected:";
		for (auto& player : internal::connection->players) {
			if (player.first != 0) { // Don't print the host as a client
				s << "\n" << player.first << "\t" << player.second.name;
			}
		}

		return s.str();
	}
	/*
	* get_players() - Return the map of the connected players
	*/
	const std::map<int,NetworkClient>& get_players() {
		return internal::connection->players;
	}

	/*
	* internal::host_send_players() - Send the player map to the given client
	* @id: the id of the given client, -1 will send to all clients
	*/
	int internal::host_send_players(int id) {
		// Convert our player socket map into a map of player names
		std::map <int,std::string> t;
		for (auto& p : internal::connection->players) { // Iterate over the connected players and insert their id and name into the new map
			t.emplace(p.first, p.second.name);
		}

		std::pair<size_t,Uint8*> player_map = network_map_encode(t); // Encode the player map as a series of Uint8's

		// See Network Message Format at the top of this file for details
		auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
			internal::connection->self_id,
			3, // Info request signal
			2, // Info type 2: player map
			player_map.first, // Data size
			player_map.second // Data
		));

		std::vector<int> ids;
		if (id < 1) {
			for (auto& player : internal::connection->players) {
				if (player.first != 0) {
					ids.push_back(player.first);
				}
			}
		} else {
			ids.push_back(id);
		}

		bool has_failed = false;
		for (auto& i : ids) {
			NetworkClient& c = internal::connection->players[i];

			if (send_packet(c, p) == 0) { // Send the entire message
				network_udp_close(&c.sock);
				internal::connection->players.erase(i);
				has_failed = true;

				messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not send player map");
				continue;
			}
			c.last_recv = get_ticks();
		}

		free(player_map.second); // Free the allocated space

		return (int)has_failed;
	}
	/*
	* internal::host_send_data() - Send the data map to the given client
	* @id: the id of the given client, -1 will send to all clients
	*/
	int internal::host_send_data(int id) {
		std::pair<size_t,Uint8*> data_map = network_map_encode(internal::connection->data); // Encode the data map as a series of Uint8's

		// See Network Message Format at the top of this file for details
		auto p = std::unique_ptr<NetworkPacket>(new NetworkPacket(
			internal::connection->self_id,
			3, // Info request signal
			4, // Info type 3: data map
			data_map.first, // Data size
			data_map.second // Data
		));

		std::vector<int> ids;
		if (id < 1) {
			for (auto& player : internal::connection->players) {
				if (player.first != 0) {
					ids.push_back(player.first);
				}
			}
		} else {
			ids.push_back(id);
		}

		bool has_failed = false;
		for (auto& i : ids) {
			NetworkClient& c = internal::connection->players[i];

			if (send_packet(c, p) == 0) { // Send the entire message
				network_udp_close(&c.sock);
				internal::connection->players.erase(i);
				has_failed = true;

				messenger::send({"engine", "network"}, E_MESSAGE::WARNING, "Could not send data map");
				continue;
			}
			c.last_recv = get_ticks();
		}

		free(data_map.second); // Free the allocated space

		return (int)has_failed;
	}
}}

#endif // BEE_NETWORK
