/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_NETWORKING
#define BEE_UTIL_NETWORKING 1

// Networking functions

#include <iostream>
#include <string.h> // Required for Windows memcpy()

#include "networking.hpp" // Include the function definitions

#include "platform.hpp" // Include the required inet_ntop() function

/*
* network_init() - Initialize SDL's networking functionality
*/
int network_init() {
	return SDLNet_Init(); // Return the attempt to initiliaze networking
}
/*
* network_close() - Close and clean up the networking system
*/
int network_close() {
	SDLNet_Quit(); // Close networking
	return 0; // Return 0 on success
}

/*
* network_resolve_host() - Return an IP address data struct based on the given IP and port
* @ip: the IP address to resolve
* @port: the port the use for the host
*/
IPaddress* network_resolve_host(const std::string& ip, int port) {
	IPaddress* ipa = new IPaddress(); // Allocate a new IPaddress struct

	int r = -1;
	if (ip.empty()) { // Resolve the data for a user acting as a host
		r = SDLNet_ResolveHost(ipa, nullptr, port); // Attempt to fill the struct
	} else { // Resolve the data for a user connecting to a host
		r = SDLNet_ResolveHost(ipa, ip.c_str(), port); // Attempt to fill the struct
	}

	if (r != 0) { // If the attempt to fill the struct failed
		std::cerr << "NET ERR Failed to resolve host: " << SDLNet_GetError() << "\n"; // Output the error message
		return nullptr; // Return nullptr on failure
	}

	return ipa; // Return the struct on success
}
/*
* network_get_address() - Return the string of an IP address based on its data in Network Byte Order
* @a: the IP address data
*/
std::string network_get_address(Uint32 a) {
	return bee_inet_ntop(&a); // Call a cross-platform version of this function from bee/util/platform.hpp
}
/*
* network_get_address() - Return the port based on its data in Network Byte Order
* @p: the port data
*/
int network_get_port(Uint16 p) {
	return SDLNet_Read16(&p);
}

/*
* network_tcp_open() - Open a TCP connection with the given IP address data
* ! Note that this will open a listen socket if the IP address is empty but the port is not
* ! Note that TCPsocket is a pointer internally
* @ipa: the IP address data
*/
TCPsocket network_tcp_open(IPaddress* ipa) {
	return SDLNet_TCP_Open(ipa); // Return the attempt to open the socket
}
/*
* network_tcp_open() - Open a TCP connection with the given IP address data
* ! When the function is called without a data struct, simply call it with a temporary one
* @ip: the IP address to use
* @port: the port to use
*/
TCPsocket network_tcp_open(const std::string& ip, int port) {
	IPaddress* ipa = network_resolve_host(ip, port);
	TCPsocket t = network_tcp_open(ipa);
	delete ipa;
	return t;
}
/*
* network_tcp_close() - Close the given TCP socket
* @tcp: the socket to close
*/
int network_tcp_close(TCPsocket* tcp) {
	SDLNet_TCP_Close(*tcp); // Close the socket
	*tcp = nullptr; // Clear the pointer
	return 0; // Return 0 on success
}
/*
* network_tcp_accept() - Accept a connection on the given socket and return the socket data
* @listen: the socket to accept a connection on
*/
TCPsocket network_tcp_accept(TCPsocket* listen) {
	return SDLNet_TCP_Accept(*listen); // Return the socket data on success
}
/*
* network_get_peer_address() - Return the IP data of the remote TCP connection
* @tcp: the remote connection to return data for
*/
IPaddress* network_get_peer_address(TCPsocket* tcp) {
	return SDLNet_TCP_GetPeerAddress(*tcp); // Return the attempt to get the peer address info
}
/*
* network_tcp_send() - Send data of a given length via TCP over the given socket
* @tcp: the socket to send the data through
* @data: the data to send
* @len: the length of the data
*/
int network_tcp_send(TCPsocket* tcp, const void* data, int len) {
	return SDLNet_TCP_Send(*tcp, data, len); // Return the attempt to send the data
}
/*
* network_tcp_send() - Send the given string via TCP over the given socket
* ! When the function is called with a string, simply call it with its C-string
* @tcp: the socket to send the data through
* @str: the string to send
*/
int network_tcp_send(TCPsocket* tcp, const std::string& str) {
	return network_tcp_send(tcp, str.c_str(), str.length()+1);
}
/*
* network_tcp_recv() - Receive data via TCP from the given socket
* @tcp: the socket to receive data from
* @data: the pointer to store the data at
* @maxlen: the maximum length of data to receive
*/
int network_tcp_recv(TCPsocket* tcp, void* data, int maxlen) {
	return SDLNet_TCP_Recv(*tcp, data, maxlen); // Return the attempt to receive the data
}
/*
* network_tcp_recv() - Receive a string via TCP from the given socket
* ! When the function is called without a pointer location, simply call it and return the data as a string
* @tcp: the socket to receive data from
* @maxlen: the maximum length of data to receive
*/
std::string network_tcp_recv(TCPsocket* tcp, int maxlen) {
	char* data = new char[maxlen]; // Initialize a new character array to temporarily store the data

	if (network_tcp_recv(tcp, data, maxlen) <= 0) { // Attempt to receive the data
		return ""; // Return an empty string on failure
	}

	std::string d (data); // Convert the data to a string and free the temporary character array
	delete[] data;

	return d; // Return the string on success
}

/*
* network_udp_open() - Open a UDP socket on the given port
* ! Note that UDP sockets may be used for both sending and receiving data, just not at the same time
* ! Note that UDPsocket is a pointer internally
* @port: the port to use
*/
UDPsocket network_udp_open(int port) {
	return SDLNet_UDP_Open(port); // Return the attempt to open the socket
}
/*
* network_udp_open_range() - Attempt to open the given UDP socket on the given port range
* ! The range that will be tried is [port, range)
* @port: the port to start with
* @range: the range to try
*/
std::pair<int,UDPsocket> network_udp_open_range(int port, size_t range) {
	UDPsocket udp = nullptr;
	size_t i;
	for (i=0; (udp == nullptr) && i<range; ++i) {
		udp = network_udp_open(port+i);
	}
	return std::make_pair(port+i, udp);
}
/*
* network_udp_close() - Close the given UDP socket
* @udp: the socket to close
*/
int network_udp_close(UDPsocket* udp) {
	SDLNet_UDP_Close(*udp); // Close the socket
	*udp = nullptr; // Clear the pointer
	return 0; // Return 0 on success
}
/*
* network_udp_reopen() - Close and reopen the given UDP socket on the given port
* @udp: the socket to recreate
* @port: the port to use
*/
int network_udp_reopen(UDPsocket* udp, int port) {
	if (*udp != nullptr) { // If the socket needs to be closed
		network_udp_close(udp); // Close the socket
	}
	*udp = network_udp_open(port); // Open the socket on the given port
	return 0; // Return 0 on success
}
/*
* network_udp_bind() - Bind the given socket to the given IP address data using the given channel
* @udp: the socket to bind
* @channel: the channel to use, set it to -1 in order to use the first available channel
* @ipa: the IP address data
*/
int network_udp_bind(UDPsocket* udp, int channel, IPaddress* ipa) {
	return SDLNet_UDP_Bind(*udp, channel, ipa); // Return the attempt to bind the socket to the given IP address
}
/*
* network_udp_bind() - Bind the given socket to the given IP address data using the given channel
* ! When the function is called with a string and port number, simply call it after resolving the host
* @udp: the socket to bind
* @channel: the channel to use
* @ip: the IP address to bind to
* @port: the port to use
*/
int network_udp_bind(UDPsocket* udp, int channel, const std::string& ip, int port) {
	IPaddress* ipa = network_resolve_host(ip, port);
	if (ipa == nullptr) {
		return -1;
	}
	int c = network_udp_bind(udp, channel, ipa);
	delete ipa;
	return c;
}
/*
* network_udp_bind() - Bind the given socket to the given IP address data using the given channel
* ! When the function is called with a string, simply call it after separating the address from the port
* @udp: the socket to bind
* @channel: the channel to use
* @ipp: the string containing the IP in the format "127.0.0.1:80"
*/
int network_udp_bind(UDPsocket* udp, int channel, const std::string& ipp) {
	std::string ip; int port;
	ip = ipp.substr(0, ipp.find(":")); // Set the IP address as the string section before the colon
	port = bee_stoi(ipp.substr(ipp.find(":"))); // Set the port as the string section after the colon
	return network_udp_bind(udp, channel, ip, port);
}
/*
* network_udp_unbind() - Unbind the given socket from the given channel and close it
* @udp: the socket to unbind
* @channel: the channel to unbind from
*/
int network_udp_unbind(UDPsocket* udp, int channel) {
	SDLNet_UDP_Unbind(*udp, channel); // Unbind the socket
	return 0; // Return 0 on success
}
/*
* network_get_peer_address() - Return the IP data of the remote UDP connection
* @udp: the remote connection to return data for
* @channel: the channel of the remote connection
*/
IPaddress* network_get_peer_address(UDPsocket udp, int channel) {
	return SDLNet_UDP_GetPeerAddress(udp, channel); // Return the attempt to get the peer address info
}
/*
* network_udp_send() - Send packet data via UDP over the given channel
* @udp: the socket to send the data through
* @channel: the channel to use
* @packet: the data to send
*/
int network_udp_send(UDPsocket udp, int channel, UDPpacket* packet) {
	return SDLNet_UDP_Send(udp, channel, packet); // Return the attempt to send the data
}
/*
* network_udp_send() - Send data via UDP over the given channel
* ! When the function is called without a packet, simply call it with a temporary one
* @udp: the socket to send the data through
* @channel: the channel to use
* @data_size: the size of the data
* @data: the data to send as a packet
*/
int network_udp_send(UDPsocket udp, int channel, size_t data_size, const Uint8* data) {
	UDPpacket* d = network_packet_alloc(data_size); // Allocate space for the data packet
	memcpy(d->data, data, data_size); // Set the packet data
	d->len = data_size;

	int r = network_udp_send(udp, channel, d); // Send the packet

	network_packet_free(d); // Free the packet

	return r; // Return the amount of destinations the data was sent to on success, or -1 on failure
}
/*
* network_udp_recv() - Receive packet data via UDP from the given socket
* @udp: the socket to receive data from
* @packet: the pointer to store that data at
*/
int network_udp_recv(UDPsocket udp, UDPpacket* packet) {
	return SDLNet_UDP_Recv(udp, packet); // Return the attempt to receive the data
}
/*
* network_udp_sendv() - Send multiple data packets via UDP
* @udp: the socket to send the data through
* @packets: a pointer to an array of packet data
* @amount: the amount of packets in the array
*/
int network_udp_send_vector(UDPsocket udp, UDPpacket** packets, int amount) {
	return SDLNet_UDP_SendV(udp, packets, amount); // Return the attempt to send the data, the total number of packets
}
/*
* network_udp_recvv() - Receive multiple data packets via UDP
* @udp: the socket to receive the data from
* @packets: the pointer to store the data at
*/
int network_udp_recv_vector(UDPsocket udp, UDPpacket** packets) {
	return SDLNet_UDP_RecvV(udp, packets); // Return the attempt to receive the data
}

/*
* network_packet_alloc() - Allocate space for packet data of a given size
* @size: the size of packet to allocate
*/
UDPpacket* network_packet_alloc(int size) {
	UDPpacket* packet = SDLNet_AllocPacket(size); // Attempt to allocate the requested space
	if (packet == nullptr) { // If the packet could not be allocated
		std::cerr << "NET ERR Failed to allocate UDP packet: " << SDLNet_GetError() << "\n"; // Output the error message
	}

	return packet; // Return the packet pointer on success
}
/*
* network_packet_resize() - Resize the packet data to the given size
* @packet: the packet to resize
* @size: the new size of the packet
*/
UDPpacket* network_packet_resize(UDPpacket* packet, int size) {
	int newsize = 0; // Define the resultant size of the packet

	newsize = SDLNet_ResizePacket(packet, size); // Attempt to resize the packet
	if (newsize < size) { // If the packet could not be resized
		std::cerr << "NET ERR Failed to resize UDP packet: " << SDLNet_GetError() << "\n"; // Output the error message
		return nullptr; // Return nullptr on failure
	}

	return packet; // Return the packet pointer on success
}
/*
* network_packet_free() - Free the packet data
* @packet: the packet to free
*/
int network_packet_free(UDPpacket* packet) {
	SDLNet_FreePacket(packet); // Free the packet
	return 0; // Return 0 on success
}
/*
* network_packet_realloc() - Reallocate space for the packet data
* ! Perhaps use this function if network_packet_resize() fails
* ! Note that after this is called, packet will be nullptr and the new packet will be returned
* ! This allows it to be used as follows:
*   	packet = network_packet_realloc(packet, size)
* @packet: the packet to reallocate
* @size: the new size of the packet
*/
UDPpacket* network_packet_realloc(UDPpacket* packet, int size) {
	network_packet_free(packet); // Free the packet
	return network_packet_alloc(size); // Return the allocated packet
}
/*
* network_packet_alloc_vector() - Allocate space for an array of packet data
* @amount: the amount of packets to allocate in the array
* @size: the size of each packet
*/
UDPpacket** network_packet_alloc_vector(int amount, int size) {
	UDPpacket** packets = SDLNet_AllocPacketV(amount, size); // Attempt to allocate space for the packets
	if (packets == nullptr) { // If the packets could not be allocated
		std::cerr << "NET ERR Failed to allocate UDP vector packets: " << SDLNet_GetError() << "\n"; // Output the error message
	}

	return packets; // Return the packet array pointer on success
}
/*
* network_packet_free_vector() - Free the packet array data
* @packets: the packet array to free
*/
int network_packet_free_vector(UDPpacket** packets) {
	SDLNet_FreePacketV(packets); // Free the packets
	return 0; // Return 0 on success
}

#endif // _BEE_UTIL_NETWORKING
