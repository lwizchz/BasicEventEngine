/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_NETWORKING_H
#define BEE_UTIL_NETWORKING_H 1

#include <string> // Include the required library headers

#include <SDL2/SDL_net.h> // Include the SDL2 networking headers

namespace util { namespace network {

int init();
void close();

IPaddress* resolve_host(const std::string&, int);
std::string get_address(Uint32);
int get_port(Uint16);

TCPsocket tcp_open(const std::string&, int);
void tcp_close(TCPsocket*);
TCPsocket tcp_accept(TCPsocket*);
IPaddress* get_peer_address(TCPsocket*);
int tcp_send(TCPsocket*, const void*, int);
int tcp_send(TCPsocket*, const std::string&);
int tcp_recv(TCPsocket*, void*, int);
std::string tcp_recv(TCPsocket*, int);

UDPsocket udp_open(int);
std::pair<int,UDPsocket> udp_open_range(int, size_t);
void udp_close(UDPsocket*);
void udp_reopen(UDPsocket*, int);
int udp_bind(UDPsocket*, int, IPaddress*);
int udp_bind(UDPsocket*, int, const std::string&, int);
void udp_unbind(UDPsocket*, int);
IPaddress* get_peer_address(UDPsocket, int);
int udp_send(UDPsocket, int, UDPpacket*);
int udp_send(UDPsocket, int, size_t, const Uint8*);
int udp_recv(UDPsocket, UDPpacket*);
int udp_send_vector(UDPsocket, UDPpacket**, int);
int udp_recv_vector(UDPsocket, UDPpacket**);

UDPpacket* packet_alloc(int);
UDPpacket* packet_resize(UDPpacket*, int);
void packet_free(UDPpacket*);
UDPpacket* packet_realloc(UDPpacket*, int);
UDPpacket** packet_alloc_vector(int, int);
void packet_free_vector(UDPpacket**);

}}

#endif // BEE_UTIL_NETWORKING_H
