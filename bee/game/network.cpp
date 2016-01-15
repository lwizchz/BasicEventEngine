/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
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
		1	server name
		2	player map
		3	data update
*/

/*
* BEE::net_init() - Initialize the networking utilities
*/
int BEE::net_init() {
	if (!options->is_network_enabled) {
		return 1; // Return 1 if networking is disabled
	}

	if (network_init()) { // Attempt to initialize networking
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

	if (network_close()) { // Attempt to close networking
		return 2; // Return 2 on failure to close
	}
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

	if (network_packet_realloc(net->udp_data, 8)) { // Attempt to allocate space to receive data
		return 2; // Return 2 on failed to allocate
	}

	int r = network_udp_recv(net->udp_recv, net->udp_data); // Attempt to receive data over the UDP socket
	if (r == -1) {
		return 3; // Return 3 on failure while receiving
	}
	if (r == 0) {
		return 0; // Return 0 when there's no data to receive
	}

	if (net->is_host) { // Handle session hosting signals
		switch (net->udp_data->data[2]) {
			case 1: { // Connection requested
				if (net->players.size() < net->max_players) { // If there is still room for clients then allow the current one to connect
					int id = net->players.size(); // Get an id for the client

					UDPsocket sock = NULL;
					int c = network_udp_bind(sock, -1, &net->udp_data->address); // Bind a sending socket to the client who is requesting a connection

					network_udp_send(sock, c, 0, 1, id); // Send the client their id

					net->players.insert(std::pair<int,UDPsocket>(id, sock)); // Add the client to the list of clients

					std::cout << "Net: client accepted\n"; // Output success string
				}
				break;
			}
			case 2: { // Client disconnected
				network_udp_close(net->players[net->udp_data->data[1]]); // Close the sending socket to the client who has disconnected

				net->players.erase(net->udp_data->data[1]); // Remove them from the list of clients

				std::cout << "Net: client disconnected\n"; // Output a success string

				break;
			}
			case 3: { // Server info request
				switch (net->udp_data->data[3]) {
					case 1: { // Client requesting server name
						Uint8* n = orda(net->name); // Convert the server name to Uint8's
						Uint8* data = (Uint8*)malloc(4+net->name.length()); // Allocate space for the signals and server name

						// See Network Message Format at the top of this file for details
						data[0] = 4+net->name.length(); // Total message length
						data[1] = 0; // Server id
						data[2] = 3; // Info request signal
						data[3] = 1; // Info type signal
						memcpy(data+4, n, net->name.length()); // Add the server name to the rest of the data

						network_udp_send(net->players[net->udp_data->data[1]], -1, data); // Send the entire message

						free(data); // Free the allocated space
						free(n);

						break;
					}
					case 2: { // Client requesting player map
						// Convert our player socket map into a map of player IP addresses
						std::map <int,std::string> t;
						for (auto& p : net->players) { // Iterate over the connected players and insert their id and IP address into the new map
							IPaddress* ipa = network_get_peer_address(p.second, -1);
							if (ipa != NULL) {
								t.insert(std::pair<int,std::string>(p.first, network_get_address(ipa->host)));
							}
						}

						Uint8* m = network_map_encode(t); // Encode the player map as a series of Uint8's
						Uint8* data = (Uint8*)malloc(4+m[0]); // Allocate space for the signals and the map data

						// See Network Message Format at the top of this file for details
						data[0] = 4+m[0]; // Total message length
						data[1] = 0; // Server id
						data[2] = 3; // Info request signal
						data[3] = 2; // Info type signal
						memcpy(data+4, m, m[0]); // Add the map to the rest of the data

						network_udp_send(net->players[net->udp_data->data[1]], -1, data); // Send the entire message

						free(data); // Free the allocated space
						free(m);

						break;
					}
					case 3: { // Client requesting data map
						Uint8* m = network_map_encode(net->data); // Encode the data map as a series of Uint8's
						Uint8* data = (Uint8*)malloc(4+m[0]); // Allocate space for the signals and the map data

						// See Network Message Format at the top of this file for details
						data[0] = 4+m[0]; // Total message length
						data[1] = 0; // Server id
						data[2] = 3; // Info request signal
						data[3] = 3; // Info type signal
						memcpy(data+4, m, m[0]); // Add the map to the rest of the data

						network_udp_send(net->players[net->udp_data->data[1]], -1, data); // Send the entire message

						free(data); // Free the allocated space
						free(m);

						break;
					}
				}
				break; // Break from signal1: 3 info requested
			}
		}
	} else { // Handle client signals
		switch (net->udp_data->data[2]) {
			case 1: { // Connection accepted
				net->self_id = net->udp_data->data[3]; // Read the id that the server assigned to us
				net->is_connected = true; // Mark our networking as connected
				std::cout << "Net: connected with id " << net->udp_data->data[3] << "\n"; // Output success message
				break;
			}
			case 2: { // Disconnected by host
				net->is_connected = false; // Mark our networking as unconnected
				network_udp_close(net->udp_send); // Close our sockets which pointed at the host
				network_udp_close(net->udp_recv);
				break;
			}
			case 3: { // Server info received
				switch (net->udp_data->data[3]) {
					case 1: { // Received server name (along with IP address)
						std::string ip = network_get_address(net->udp_data->address.host); // Get the server IP
						std::string name = chra(net->udp_data->data+4); // Get the server name from the data

						net->servers.insert(std::make_pair(ip, name)); // Add the server to the list of available servers

						break;
					}
					case 2: { // Received player map
						std::map <std::string,std::string> t; // Temp map to store IP addresses in
						net->players.clear(); // Remove all players in order to clean out old connections

						network_map_decode(net->udp_data->data+4, t); // Decode the player map from the data into t
						for (auto& p : t) { // Iterate over the players in the temp map in order to generate sockets for each of them
							UDPsocket sock = NULL;
							network_udp_bind(sock, -1, p.second); // Bind a socket to the player's IP address

							net->players.insert(std::pair<int,UDPsocket>(std::stoi(p.first), sock)); // Insert the player's id and IP address into our copy of the player map
						}

						break;
					}
					case 3: { // Received data map
						network_map_decode(net->udp_data->data+4, net->data); // Decode the data map directly into net->data in order to immediately replace all old data
						break;
					}
				}
				break; // Break from signal1: 3 info received
			}
		}
	}

	return 0; // Return 0 on success
}

/*
* BEE::net_session_start() - Begin hosting a network session for others to join over UDP
* @session_name: the name that the session will identify as
* @max_players: the maximum amount of people connected to the session including the host
* @player_name: the name of the player who's hosting
*/
int BEE::net_session_start(const std::string& session_name, int max_players, const std::string& player_name) {
	net->players.clear(); // Clear all old player data

	net->udp_recv = network_udp_open(net->id); // Open a UDP listening socket to receive from all clients
	if (net->udp_recv == NULL) {
		return 1; // Return 1 if the socket failed to open
	}

	// Set the session info
	net->name = session_name;
	net->max_players = max_players;
	net->self_id = 0;
	net->players.insert(std::pair<int,UDPsocket>(net->self_id, NULL)); // Insert ourself into the player map

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

	net->udp_recv = network_udp_open(net->id); // Open a UDP listening socket to receive responses from the servers
	if (net->udp_recv == NULL) {
		std::cerr << "Net: net_session_find() : UDP listening socket failed to open\n"; // Output error string
		return net->servers; // Return an empty map if the receiving socket failed to open
	}

	net->channel = network_udp_bind(net->udp_send, -1, "255.255.255.255", net->id); // Bind a sending socket to the broadcast IP 255.255.255.255
	if (net->channel == -1) {
		network_udp_close(net->udp_recv); // Close the receiving socket

		std::cerr << "Net: net_session_find() : UDP broadcast socket failed to bind\n"; // Output error string
		return net->servers; // Return an empty map if the sending socket failed to bind
	}

	network_udp_send(net->udp_send, net->channel, net->self_id, 3, 0); // Send a server name info request

	if (network_packet_realloc(net->udp_data, 8)) { // Attempt to allocate space to receive data
		network_udp_close(net->udp_recv); // Close the sockets
		network_udp_close(net->udp_send);

		std::cerr << "Net: net_session_find() : Failed to allocate space to receive data\n";
		return net->servers; // Return an empty map if the allocation failed
	}

	Uint32 t = get_ticks(); // Get the current time
	int r = network_udp_recv(net->udp_recv, net->udp_data); // Attempt to receive data
	while (get_ticks() - t < net->timeout) { // Continue receiving until we timeout
		if ((r == 1)&&(net->udp_data->data[2] == 3)) { // If data was received and it is a server name info response
			std::string name = chra(net->udp_data->data+4); // Get the server name from the data
			net->servers.insert(std::make_pair(network_get_address(net->udp_data->address.host), name)); // Add the server to the list of available servers
		}
		r = network_udp_recv(net->udp_recv, net->udp_data); // Attempt to receive more data
	}

	network_udp_close(net->udp_recv); // Close the receiving socket
	network_udp_close(net->udp_send); // Close the sending socket
	return net->servers; // Return the filled map on success
}
/*
* BEE::net_session_join() - Attempt to join a session at the given IP address
* @ip: the IP address to attempt to connect to
* @player_name: the name of the player which will be sent to the host
*/
int BEE::net_session_join(const std::string& ip, const std::string& player_name) {
	net->players.clear(); // Clear the previous player map

	net->udp_recv = network_udp_open(net->id); // Open a UDP listening socket to receive messages from the server
	if (net->udp_recv == NULL) {
		return 1; // Return 1 if the receiving socket failed to open
	}

	net->channel = network_udp_bind(net->udp_send, -1, ip, net->id); // Bind a sending socket to the given server IP address
	if (net->channel == -1) {
		network_udp_close(net->udp_recv); // Close the receiving socket

		return 2; // Return 2 if the sending socket failed to bind
	}

	network_udp_send(net->udp_send, net->channel, 0, 1, 0); // Send a server connection request

	if (network_packet_realloc(net->udp_data, 8)) { // Attempt to allocate space to receive data
		network_udp_close(net->udp_recv); // Close the sockets
		network_udp_close(net->udp_send);

		return 3; // Return 3 if the allocation failed
	}

	Uint32 t = get_ticks(); // Get the current time
	int r = network_udp_recv(net->udp_recv, net->udp_data); // Attempt to receive data
	while ((r < 1)||(get_ticks() - t < net->timeout)) { // Wait to receive data until we timeout
		r = network_udp_recv(net->udp_recv, net->udp_data); // Attempt to receive data
	}

	if ((r < 1)||(net->udp_data->data[0] != 1)) { // If no data was received or if the data was not a connection response
		network_udp_close(net->udp_recv); // Close the sockets
		network_udp_close(net->udp_send);

		std::cerr << "Net: net_session_join() : Failed to connect to " << ip << "\n"; // Output error string
		return 4; // Return 4 on failed to connect
	}

	// Set connection info
	net->self_id = net->udp_data->data[1];
	net->is_connected = true;

	std::cout << "Net: net_session_join() : Connected with id " << net->udp_data->data[1] << "\n"; // Output success string

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
	if (net->is_host) { // If we are the host
		for (auto& p : net->players) { // Iterate over the clients to disconnect them
			network_udp_send(p.second, net->channel, net->self_id, 2, 0); // Send a disconnection signal
			network_udp_close(p.second); // Close the socket
		}

		// Reset connection info
		net->is_host = false;
	} else {
		network_udp_send(net->udp_send, net->channel, net->self_id, 2, net->self_id); // Send a disconnection signal to the server
	}

	// Close sockets and free data
	network_udp_close(net->udp_send);
	network_udp_close(net->udp_recv);
	network_packet_free(net->udp_data);

	// Reset connection and client info
	net->is_connected = false;
	net->servers.clear();
	net->name.clear();
	net->players.clear();
	net->data.clear();

	return 0; // Return 0 on success
}

#endif // _BEE_GAME_NETWORK
