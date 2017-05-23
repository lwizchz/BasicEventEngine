/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_NETWORK_H
#define _BEE_UTIL_NETWORK_H 1

#include <string> // Include the required library headers
#include <iostream>

#include <SDL2/SDL_net.h> // Include the SDL2 networking headers

#include "platform.hpp" // Include the required inet_ntop() function

int network_init();
int network_close();
IPaddress* network_resolve_host(const std::string&, int);
std::string network_get_address(Uint32);
int network_get_port(Uint16);
TCPsocket network_tcp_open(const std::string&, int);
int network_tcp_close(TCPsocket*);
TCPsocket network_tcp_accept(TCPsocket*);
IPaddress* network_get_peer_address(TCPsocket*);
int network_tcp_send(TCPsocket*, const void*, int);
int network_tcp_send(TCPsocket*, const std::string&);
int network_tcp_recv(TCPsocket*, void*, int);
std::string network_tcp_recv(TCPsocket*, int);
UDPsocket network_udp_open(int);
UDPsocket network_udp_open_range(int, size_t);
int network_udp_close(UDPsocket*);
int network_udp_reopen(UDPsocket*, int);
int network_udp_bind(UDPsocket*, int, IPaddress*);
int network_udp_bind(UDPsocket*, int, const std::string&, int);
int network_udp_bind(UDPsocket*, int, const std::string&);
int network_udp_unbind(UDPsocket*, int);
IPaddress* network_get_peer_address(UDPsocket, int);
int network_udp_send(UDPsocket, int, UDPpacket*);
int network_udp_recv(UDPsocket, UDPpacket*);
int network_udp_send_vector(UDPsocket, UDPpacket**, int);
int network_udp_recv_vector(UDPsocket, UDPpacket**);
int network_udp_send(UDPsocket, int, Uint8*);
int network_udp_send(UDPsocket, int, Uint8, Uint8, Uint8);
UDPpacket* network_packet_alloc(int);
UDPpacket* network_packet_resize(UDPpacket*, int);
int network_packet_free(UDPpacket*);
UDPpacket* network_packet_realloc(UDPpacket*, int);
UDPpacket** network_packet_alloc_vector(int, int);
int network_packet_free_vector(UDPpacket**);

#endif // _BEE_UTIL_NETWORK_H
