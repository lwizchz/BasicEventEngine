/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_NETWORK
#define _BEE_GAME_NETWORK 1

#include "../game.hpp" // Include the engine headers

/*
	Network message format {
		total message length,
		sender id,
		signal1,
		signal2, (if applicable)
		data
	}
*/

/*
	Network signals
	1	connect requested/accepted
	2	disconnect notification
	3	server info request
		0	keep alive
		1	server name
		2	player map
		3	data update
		4	data commit
*/

/*
* BEE::net_init() - Initialize the networking utilities
*/
int BEE::net_init() {
	if (!options->is_network_enabled) {
		return 1; // Return 1 if networking is disabled
	}

	if (network_init()) { // Attempt to initialize networking
		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Error initializing network functionality: " + get_sdl_error());
		return 2; // Return 2 on failure to init
	}
	net->is_initialized = true; // Mark the network as ready

	return 0; // Return 0 on success
}
/*
* BEE::net_get_is_initialized() - Return whether networking has been initialized or not
*/
bool BEE::net_get_is_initialized() const {
	return net->is_initialized;
}
/*
* BEE::net_close() - Close and clean up the networking utilities
*/
int BEE::net_close() {
	if (!options->is_network_enabled) {
		return 1; // Return 1 if networking is disabled
	}

	network_packet_free(net->udp_data); // Free the space used to receive data
	net->udp_data = nullptr;

	network_close(); // Close networking
	net->is_initialized = false; // Mark the network as not ready
	delete net; // Reinitialize the NetworkData in case the user wants to restart networking
	net = new NetworkData();

	return 0; // Return 0 on success
}
/*
* BEE::net_handle_events() - Handle network sending and receiving during the event loop
*/
int BEE::net_handle_events() {
	if ((!options->is_network_enabled)||(!net->is_initialized)) {
		return 1; // Return 1 if networking is disabled or if it has not been initialized
	}

	net->udp_data = network_packet_realloc(net->udp_data, 256); // Attempt to allocate space to receive data
	if (net->udp_data == nullptr) {
		return 2; // Return 2 on failed to allocate
	}

	int r = network_udp_recv(net->udp_sock, net->udp_data); // Attempt to receive data over the UDP socket
	while (r == 1) { // Iterate over the received packets
		if (net->udp_data->len < 3) {
			continue; // Continue when the data doesn't contain a signal
		}

		if (net->is_host) { // Handle session hosting signals
			switch (net->udp_data->data[2]) {
				case 1: { // Connection requested
					if (net->players.size() < net->max_players) { // If there is still room for clients then allow the current one to connect
						int id = net->players.size(); // Get an id for the client

						NetworkClient c;
						c.sock = network_udp_open_range(net->id, net->max_players);
						if (c.sock == nullptr) {
							messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not accept client");
							break;
						}

						c.channel = network_udp_bind(&c.sock, -1, &net->udp_data->address); // Bind a sending socket to the client who is requesting a connection
						if (c.channel == -1) {
							network_udp_close(&c.sock);

							messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not accept client");
							break;
						}

						if (network_udp_send(c.sock, c.channel, net->self_id, 1, id) == 0) { // Send the client their id
							network_udp_close(&c.sock);

							messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not accept client");
							break;
						}
						c.last_recv = get_ticks();

						net->players.emplace(id, c); // Add the client to the list of clients

						messenger_send({"engine", "network"}, BEE_MESSAGE_INFO, "Client accepted");
					}
					break;
				}
				case 2: { // Client disconnected
					NetworkClient& c = net->players[net->udp_data->data[1]];
					network_udp_unbind(&c.sock, c.channel); // Unbind the socket to the client who has disconnected
					network_udp_close(&c.sock); // Close the socket

					net->players.erase(net->udp_data->data[1]); // Remove them from the list of clients

					messenger_send({"engine", "network"}, BEE_MESSAGE_INFO, "Client disconnected");

					break;
				}
				case 3: { // Server info request
					switch (net->udp_data->data[3]) {
						case 0: {
							NetworkClient& c = net->players[net->udp_data->data[1]];
							c.last_recv = get_ticks();
							break;
						}
						case 1: { // Client requesting server name
							Uint8* n = orda(net->name); // Convert the server name to Uint8's
							Uint8* data = (Uint8*)malloc(4+net->name.length()); // Allocate space for the signals and server name

							// See Network Message Format at the top of this file for details
							data[0] = 4+net->name.length(); // Total message length
							data[1] = net->self_id; // Server id
							data[2] = 3; // Info request signal
							data[3] = 1; // Info type signal
							memcpy(data+4, n, net->name.length()); // Add the server name to the rest of the data

							UDPsocket sock = network_udp_open_range(net->id, net->max_players+1);
							if (sock == nullptr) {
								messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not open socket");
								break;
							}

							if (network_udp_send(sock, -1, data) == 0) { // Send the entire message
								network_udp_close(&sock);

								messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not send server info");
								break;
							}

							network_udp_close(&sock);

							free(data); // Free the allocated space
							free(n);

							break;
						}
						case 2: { // Client requesting player map
							// Convert our player socket map into a map of player IP addresses
							std::map <int,std::string> t;
							for (auto& p : net->players) { // Iterate over the connected players and insert their id and IP address into the new map
								IPaddress* ipa = network_get_peer_address(p.second.sock, -1);
								if (ipa != nullptr) {
									t.emplace(p.first, network_get_address(ipa->host));
								}
							}

							Uint8* m = network_map_encode(t); // Encode the player map as a series of Uint8's
							Uint8* data = (Uint8*)malloc(4+m[0]); // Allocate space for the signals and the map data

							// See Network Message Format at the top of this file for details
							data[0] = 4+m[0]; // Total message length
							data[1] = net->self_id; // Server id
							data[2] = 3; // Info request signal
							data[3] = 2; // Info type signal
							memcpy(data+4, m, m[0]); // Add the map to the rest of the data

							NetworkClient& c = net->players[net->udp_data->data[1]];
							if (network_udp_send(c.sock, c.channel, data) == 0) { // Send the entire message
								network_udp_close(&c.sock);
								net->players.erase(net->udp_data->data[1]);

								messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not send player map");
								break;
							}
							c.last_recv = get_ticks();

							free(data); // Free the allocated space
							free(m);

							break;
						}
						case 3: { // Client requesting data map
							Uint8* m = network_map_encode(net->data); // Encode the data map as a series of Uint8's
							Uint8* data = (Uint8*)malloc(4+m[0]); // Allocate space for the signals and the map data

							// See Network Message Format at the top of this file for details
							data[0] = 4+m[0]; // Total message length
							data[1] = net->self_id; // Server id
							data[2] = 3; // Info request signal
							data[3] = 3; // Info type signal
							memcpy(data+4, m, m[0]); // Add the map to the rest of the data

							NetworkClient& c = net->players[net->udp_data->data[1]];
							if (network_udp_send(c.sock, -1, data) == 0) { // Send the entire message
								network_udp_close(&c.sock);
								net->players.erase(net->udp_data->data[1]);

								messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not send data map");
								break;
							}
							c.last_recv = get_ticks();

							free(data); // Free the allocated space
							free(m);

							break;
						}
					}
					break; // Break from signal1: 3, info requested
				}
			}
		} else { // Handle client signals
			net->last_recv = get_ticks();
			switch (net->udp_data->data[2]) {
				case 1: { // Connection accepted
					net->self_id = net->udp_data->data[3]; // Read the id that the server assigned to us
					net->is_connected = true; // Mark our networking as connected
					messenger_send({"engine", "network"}, BEE_MESSAGE_INFO, "Connected to server with id " + bee_itos(net->self_id));
					break;
				}
				case 2: { // Disconnected by host
					net_session_end(); // Reset session
					messenger_send({"engine", "network"}, BEE_MESSAGE_INFO, "Disconnected by server");
					break;
				}
				case 3: { // Server info received
					switch (net->udp_data->data[3]) {
						case 0: {
							if (network_udp_send(net->udp_sock, net->channel, net->self_id, 3, 0) == 0) { // Send the keep alive
								messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not send keep alive to server");
								break;
							}
							break;
						}
						case 1: { // Received server name (along with IP address)
							std::string ip = network_get_address(net->udp_data->address.host); // Get the server IP
							std::string name = chra(net->udp_data->data+4); // Get the server name from the data

							net->servers.emplace(ip, name); // Add the server to the list of available servers

							break;
						}
						case 2: { // Received player map
							std::map <std::string,std::string> t; // Temp map to store IP addresses in
							net->players.clear(); // Remove all players in order to clean out old connections

							network_map_decode(net->udp_data->data+4, &t); // Decode the player map from the data into t
							for (auto& p : t) { // Iterate over the players in the temp map in order to generate sockets for each of them
								NetworkClient c;
								c.channel = network_udp_bind(&c.sock, -1, p.second); // Bind a socket to the player's IP address

								net->players.emplace(bee_stoi(p.first), c); // Insert the player's id and IP address into our copy of the player map
							}

							break;
						}
						case 3: // Received partial data map
						case 4: { // Received end of data map
							// Buffer the received data if necessary
							if (net->tmp_data_buffer == nullptr) {
								net->tmp_data_buffer = (Uint8*)malloc(net->udp_data->data[4]);
								memcpy(net->tmp_data_buffer, net->udp_data->data+4, net->udp_data->data[4]);
							} else {
								net->tmp_data_buffer = (Uint8*)realloc(net->tmp_data_buffer, net->tmp_data_buffer[0] + net->udp_data->data[4]);
								memcpy(net->tmp_data_buffer+net->tmp_data_buffer[0], net->udp_data->data+4, net->udp_data->data[4]);
							}

							if (net->udp_data->data[2] == 3) { // Break when only a partial map has been received
								break;
							}

							network_map_decode(net->tmp_data_buffer, &net->data); // Decode the data map directly into net->data in order to immediately replace all old data

							free(net->tmp_data_buffer);
							net->tmp_data_buffer = nullptr;

							break;
						}
					}
					break; // Break from signal1: 3 info received
				}
			}
		}

		net->udp_data = network_packet_realloc(net->udp_data, 256); // Attempt to allocate space to receive data
		if (net->udp_data == nullptr) {
			return 2; // Return 2 on failed to allocate
		}

		r = network_udp_recv(net->udp_sock, net->udp_data); // Attempt to receive data over the UDP socket
	}
	if (r == -1) {
		return 3; // Return 3 on failure while receiving
	}

	const Uint32 now = get_ticks();
	if (net->is_host) { // If we are the host
		for (auto& p : net->players) { // Iterate over the clients to ensure they are still connected
			if (p.first != 0) { // Don't disconnect ourselves
				if (now - p.second.last_recv > net->timeout) { // Only disconnect clients that have timed out
					network_udp_send(p.second.sock, p.second.channel, net->self_id, 2, 0); // Send a disconnection signal
					network_udp_unbind(&p.second.sock, p.second.channel); // Unbind the socket
					network_udp_close(&p.second.sock); // Close the socket
					net->players.erase(p.first); // Remove the client from the map
				} else if (now - p.second.last_recv > net->timeout/2) { // Send a keep alive to clients that might be timing out
					if (network_udp_send(p.second.sock, p.second.channel, net->self_id, 3, 0) == 0) { // Send the keep alive
						messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Could not send keep alive to client");
					}
				}
			}
		}
	} else {
		if (now - net->last_recv > net->timeout) {
			net_session_end();
		} else if (now - net->last_recv > net->timeout/2) {
			// Wait for server to send keep alive?
		}
	}

	return 0; // Return 0 on success
}
/*
* BEE::net_get_data() - Return the NetworkData struct
*/
const BEE::NetworkData& BEE::net_get_data() const {
	return *net;
}

/*
* BEE::net_session_start() - Begin hosting a network session for others to join over UDP
* @session_name: the name that the session will identify as
* @max_players: the maximum amount of people connected to the session including the host
* @player_name: the name of the player who's hosting
*/
int BEE::net_session_start(const std::string& session_name, int max_players, const std::string& player_name) {
	net_session_end(); // Reset session

	net->udp_sock = network_udp_open(net->id); // Open a UDP listening socket to receive from all clients
	if (net->udp_sock == nullptr) {
		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "Failed to start new session \"" + session_name + "\"");
		return 1; // Return 1 if the socket failed to open
	}

	messenger_send({"engine", "network"}, BEE_MESSAGE_INFO, "Started new session \"" + session_name + "\" with " + bee_itos(max_players) + " max players");

	// Set the session info
	net->name = session_name;
	net->max_players = max_players;
	net->self_id = 0;
	net->players.emplace(net->self_id, NetworkClient()); // Insert ourself into the player map

	// Set connection info
	net->is_connected = true;
	net->is_host = true;

	return 0; // Return 0 on success
}
/*
* BEE::net_session_find() - Query the local network for available sessions
*/
std::map<std::string,std::string> BEE::net_session_find() {
	net->servers.clear(); // Clear the previously available servers

	if (net->is_connected) {
		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "net_session_find() : Failed to find sessions, already connected");
		return net->servers; // Return 1 if we are already connected
	} else {
		net_session_end(); // Reset session
	}

	net->udp_sock = network_udp_open(net->id);
	net->channel = network_udp_bind(&net->udp_sock, -1, "192.168.1.255", net->id); // Bind a sending socket to the broadcast IP 192.168.1.255
	if (net->channel == -1) {
		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "net_session_find() : UDP broadcast socket failed to bind");
		return net->servers; // Return an empty map if the sending socket failed to bind
	}

	if (network_udp_send(net->udp_sock, net->channel, net->self_id, 3, 1) == 0) { // Send a server name info request
		network_udp_close(&net->udp_sock);

		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "net_session_find() : Failed to send request on port " + bee_itos(net->id));
		return net->servers; // Return an empty map if the message failed to send
	}

	net->udp_data = network_packet_realloc(net->udp_data, 256);
	if (net->udp_data == nullptr) { // Attempt to allocate space to receive data
		network_udp_close(&net->udp_sock); // Close the socket

		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "net_session_find() : Failed to allocate space to receive data");
		return net->servers; // Return an empty map if the allocation failed
	}

	Uint32 t = get_ticks(); // Get the current time
	int r = network_udp_recv(net->udp_sock, net->udp_data); // Attempt to receive data
	while (get_ticks() - t < net->timeout) { // Continue receiving until we timeout
		if (
			(r == 1)
			&&(net->udp_data->data[0] > 4)
			&&(net->udp_data->data[2] == 3)
			&&(net->udp_data->data[3] == 1)
		) { // If data was received and it is a server name info response
			std::string name = chra(net->udp_data->data+4); // Get the server name from the data
			net->servers.emplace(network_get_address(net->udp_data->address.host), name); // Add the server to the list of available servers
		}
		r = network_udp_recv(net->udp_sock, net->udp_data); // Attempt to receive more data
	}

	network_udp_close(&net->udp_sock); // Close the socket
	return net->servers; // Return the filled map on success
}
/*
* BEE::net_session_join() - Attempt to join a session at the given IP address
* @ip: the IP address to attempt to connect to
* @player_name: the name of the player which will be sent to the host
*/
int BEE::net_session_join(const std::string& ip, const std::string& player_name) {
	if (net->is_connected) {
		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "net_session_join() : Failed to join session, already connected");
		return 1; // Return 1 if we are already connected
	} else {
		net_session_end(); // Reset session
	}

	net->players.clear(); // Clear the previous player map

	net->udp_sock = network_udp_open(net->id);
	net->channel = network_udp_bind(&net->udp_sock, -1, ip, net->id); // Bind a sending socket to the given server IP address
	if (net->channel == -1) {
		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "net_session_join() : Failed to bind to " + ip + " on port " + bee_itos(net->id));
		return 2; // Return 2 if the socket failed to bind
	}

	if (network_udp_send(net->udp_sock, net->channel, 0, 1, 0) == 0) { // Send a server connection request
		network_udp_close(&net->udp_sock); // Close the socket

		messenger_send({"engine", "network"}, BEE_MESSAGE_WARNING, "net_session_join() : Failed to send request to " + ip + " on port " + bee_itos(net->id));
		return 3; // Return 3 if the messsage failed to send
	}

	return 0; // Return 0 on success
}
/*
* BEE::net_get_is_connected() - Return whether the network is currently connected to a session
*/
bool BEE::net_get_is_connected() {
	return net->is_connected;
}
/*
* BEE::net_session_end() - End the session connection
*/
int BEE::net_session_end() {
	if (net->is_connected) {
		if (net->is_host) { // If we are the host
			messenger_send({"engine", "network"}, BEE_MESSAGE_INFO, "Disconnecting " + bee_itos(net->players.size()-1) + " clients...");
			for (auto& p : net->players) { // Iterate over the clients to disconnect them
				if (p.first != 0) { // Only disconnect clients that aren't us
					network_udp_send(p.second.sock, p.second.channel, net->self_id, 2, 0); // Send a disconnection signal
					network_udp_unbind(&p.second.sock, p.second.channel); // Unbind the socket
					network_udp_close(&p.second.sock); // Close the socket
				}
			}

			// Reset connection info
			net->is_host = false;
		} else {
			network_udp_send(net->udp_sock, net->channel, net->self_id, 2, 0); // Send a disconnection signal to the server
		}
	}

	// Close socket and free data
	if (net->udp_sock != nullptr) {
		network_udp_close(&net->udp_sock);
		net->udp_sock = nullptr;
	}
	if (net->udp_data != nullptr) {
		network_packet_free(net->udp_data);
		net->udp_data = nullptr;
	}

	// Reset connection and client info
	net->is_connected = false;
	net->servers.clear();
	net->name.clear();
	net->players.clear();
	net->data.clear();

	return 0; // Return 0 on success
}

/*
* BEE::net_session_sync_data() - Sync the given data to the session
* @key: the data name
* @value: the data itself
*/
int BEE::net_session_sync_data(const std::string& key, const std::string& value) {
	net->data[key] = value;
	return 0;
}

/*
* BEE::net_session_sync_instance() - Sync the given instance to the session
*/
int BEE::net_session_sync_instance(InstanceData* inst) {
	net->data["inst_"+bee_itos(inst->id)] = inst->serialize();
	return 0;
}

#endif // _BEE_GAME_NETWORK
