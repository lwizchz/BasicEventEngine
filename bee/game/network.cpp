/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_NETWORK
#define _BEE_GAME_NETWORK 1

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
	3	server info transfer
		1	server name
		2	player map
		3	data update
*/

int BEE::net_init() {
	if (!options->is_network_enabled) {
		if (!network_init()) {
			net->is_initialized = true;
		}
	}

	return 0;
}
bool BEE::net_get_is_initialized() {
	return net->is_initialized;
}
int BEE::net_end() {
	if (!options->is_network_enabled) {
		if (!network_quit()) {
			net->is_initialized = false;
		}
	}

	return 0;
}
int BEE::net_handle_events() {
	if (network_packet_realloc(net->udp_data, 8)) {
		return 1;
	}

	int r = network_udp_recv(net->udp_recv, net->udp_data);
	if (r == -1) {
		return 1;
	}
	if (r == 0) {
		return 0;
	}

	if (net->is_host) {
		switch (net->udp_data->data[2]) {
			case 1: { // Connection requested
				UDPsocket sock;
				int c = network_udp_bind(sock, -1, &net->udp_data->address);
				int id = net->players.size();
				network_udp_send(sock, c, 0, 1, id);
				net->players.insert(std::pair<int,UDPsocket>(id, sock));
				std::cout << "Client accepted\n";
				break;
			}
			case 2: { // Client disconnected
				network_udp_close(net->players[net->udp_data->data[1]]);
				net->players.erase(net->udp_data->data[1]);
				std::cout << "Client disconnected\n";
				break;
			}
			case 3: { // Server info request
				switch (net->udp_data->data[3]) {
					case 1: { // Server name
						Uint8* data = (Uint8*)malloc(net->name.length()+4);
						data[0] = net->name.length()+4;
						data[1] = 0;
						data[2] = 3;
						data[3] = 1;
						memcpy(data+4, orda(net->name), net->name.length());
						network_udp_send(net->players[net->udp_data->data[1]], -1, data);
						free(data);
						break;
					}
					case 2: { // Player map
						std::map <int,std::string> t;
						for (auto& p : net->players) {
							t.insert(std::pair<int,std::string>(p.first, network_get_address(network_get_peer_address(p.second, -1)->host)));
						}

						Uint8* m = network_map_encode(t);
						Uint8* data = (Uint8*)malloc(m[0]+4);
						data[0] = m[0]+4;
						data[1] = 0;
						data[2] = 3;
						data[3] = 2;
						memcpy(data+4, m, m[0]);
						network_udp_send(net->players[net->udp_data->data[1]], -1, data);
						free(data);
						break;
					}
					case 3: { // Data map
						Uint8* m = network_map_encode(net->data);
						Uint8* data = (Uint8*)malloc(m[0]+4);
						data[0] = m[0]+4;
						data[1] = 0;
						data[2] = 3;
						data[3] = 3;
						memcpy(data+4, m, m[0]);
						network_udp_send(net->players[net->udp_data->data[1]], -1, data);
						free(data);
						break;
					}
				}
				break;
			}
		}
	} else {
		switch (net->udp_data->data[2]) {
			case 1: { // Connection accepted
				net->self_id = net->udp_data->data[3];
				net->is_connected = true;
				std::cout << "Connected with id " << net->udp_data->data[3] << "\n";
				break;
			}
			case 2: { // Disconnected by host
				net->is_connected = false;
				network_udp_close(net->udp_send);
				network_udp_close(net->udp_recv);
				break;
			}
			case 3: { // Server info recieved
				switch (net->udp_data->data[3]) {
					case 1: { // Server IP address and name
						std::string d = chra(net->udp_data->data+4);
						std::string ip = d.substr(0, d.find(","));
						d.erase(0, d.find(","));
						std::string name = d.substr(0, d.find(","));
						net->servers.insert(std::make_pair(ip, name));
						break;
					}
					case 2: { // Player map
						std::map <std::string,std::string> t;
						net->players.clear();
						network_map_decode(net->udp_data->data+4, t);
						for (auto& p : t) {
							UDPsocket sock;
							network_udp_bind(sock, -1, p.second);
							net->players.insert(std::pair<int,UDPsocket>(std::stoi(p.first), sock));
						}
						break;
					}
					case 3: { // Data map
						network_map_decode(net->udp_data->data+4, net->data);
						break;
					}
				}
				break;
			}
		}
	}

	return 0;
}

bool BEE::net_session_start(std::string session_name, int max_players, std::string player_name) {
	net->players.clear();

	net->udp_recv = network_udp_open(net->id);
	if (net->udp_recv == NULL) {
		return false;
	}

	net->name = session_name;
	net->max_players = max_players;
	net->self_id = 0;
	net->players.insert(std::pair<int,UDPsocket>(net->self_id, NULL));

	net->is_connected = true;
	net->is_host = true;

	return true;
}
std::map<std::string,std::string> BEE::net_session_find() {
	std::map<std::string,std::string> servers;
	servers.clear();

	net->udp_recv = network_udp_open(net->id);
	if (net->udp_recv == NULL) {
		return servers;
	}

	net->channel = network_udp_bind(net->udp_send, -1, "255.255.255.255", net->id);
	if (net->channel == -1) {
		network_udp_close(net->udp_recv);
		return servers;
	}

	network_udp_send(net->udp_send, net->channel, net->self_id, 3, 0);

	if (network_packet_realloc(net->udp_data, 8)) {
		network_udp_close(net->udp_recv);
		network_udp_close(net->udp_send);
		return servers;
	}

	Uint32 t = SDL_GetTicks();
	int r = network_udp_recv(net->udp_recv, net->udp_data);
	while (SDL_GetTicks() - t < net->timeout) {
		r = network_udp_recv(net->udp_recv, net->udp_data);
		if ((r == 1)&&(net->udp_data->data[2] == 3)) {
			std::string name = chra(net->udp_data->data+4);
			net->servers.insert(std::make_pair(network_get_address(net->udp_data->address.host), name));
		}
	}

	network_udp_close(net->udp_recv);
	network_udp_close(net->udp_send);
	return servers;
}
bool BEE::net_session_join(std::string ip, std::string player_name) {
	net->players.clear();

	net->udp_recv = network_udp_open(net->id);
	if (net->udp_recv == NULL) {
		return false;
	}

	net->channel = network_udp_bind(net->udp_send, -1, ip, net->id);
	if (net->channel == -1) {
		network_udp_close(net->udp_recv);
		return false;
	}

	network_udp_send(net->udp_send, net->channel, 0, 1, 0);

	if (network_packet_realloc(net->udp_data, 8)) {
		network_udp_close(net->udp_recv);
		network_udp_close(net->udp_send);
		return false;
	}

	Uint32 t = SDL_GetTicks();
	int r = network_udp_recv(net->udp_recv, net->udp_data);
	while ((r < 1)||(SDL_GetTicks() - t < net->timeout)) {
		r = network_udp_recv(net->udp_recv, net->udp_data);
	}

	if ((r < 1)||(net->udp_data->data[0] != 1)) {
		std::cerr << "Failed to connect to " << ip << "\n";
		network_udp_close(net->udp_recv);
		network_udp_close(net->udp_send);
		return false;
	}

	net->self_id = net->udp_data->data[1];
	net->is_connected = true;
	std::cout << "Connected with id " << net->udp_data->data[1] << "\n";

	return true;
}
bool BEE::net_get_is_connected() {
	return net->is_connected;
}
int BEE::net_session_end() {
	if (net->is_host) {
		for (auto& p : net->players) {
			network_udp_send(p.second, net->channel, net->self_id, 2, 0);
			network_udp_close(p.second);
		}
	} else {
		network_udp_send(net->udp_send, net->channel, net->self_id, 2, net->self_id);
	}

	net->is_connected = false;
	network_udp_close(net->udp_send);
	network_udp_close(net->udp_recv);

	return 0;
}

#endif // _BEE_GAME_NETWORK
