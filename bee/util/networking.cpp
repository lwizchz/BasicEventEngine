/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_NETWORKING
#define BEE_UTIL_NETWORKING 1

// Networking functions

#include <iostream>
#include <string.h> // Required for Windows memcpy()

#include "networking.hpp" // Include the function definitions

#include "platform.hpp" // Include the required inet_ntop() function

namespace util { namespace network {

/**
* Initialize SDL's networking functionality.
* @retval 0 success
* @retval -1 failed to initialize networking
*/
int init() {
	return SDLNet_Init();
}
/**
* Close and clean up the networking system.
*/
void close() {
	SDLNet_Quit();
}

/**
* @param ip the IP address to resolve
* @param port the port the use for the host
*
* @return an IP address data struct based on the given IP and port or nullptr if the host could not be resolved
*/
IPaddress* resolve_host(const std::string& ip, int port) {
	IPaddress* ipa = new IPaddress(); // Allocate a new IPaddress struct

	int r = -1;
	if (ip.empty()) { // Resolve the data for a user acting as a host
		r = SDLNet_ResolveHost(ipa, nullptr, port);
	} else { // Resolve the data for a user connecting to a host
		r = SDLNet_ResolveHost(ipa, ip.c_str(), port);
	}

	if (r != 0) { // If the attempt to fill the struct failed
		delete ipa;
		std::cerr << "UTIL NETWORK Failed to resolve host: " << SDLNet_GetError() << "\n"; // Output an error message
		return nullptr;
	}

	return ipa;
}
/**
* @param addr the IP address data
*
* @returns the string of an IP address based on its data in Network Byte Order
*/
std::string get_address(Uint32 addr) {
	return platform::inet_ntop(&addr);
}
/**
* @param port the port data
*
* @returns the port based on its data in Network Byte Order
*/
int get_port(Uint16 port) {
	return SDLNet_Read16(&port);
}

/**
* Open a TCP connection with the given IP address.
* @note This will open a listen socket if the IP address is empty but the port is not.
* @note The returned TCPsocket is a pointer typedef.
* @param ipa the IP address data
*
* @returns the opened socket
*/
TCPsocket tcp_open(IPaddress* ipa) {
	return SDLNet_TCP_Open(ipa);
}
/**
* Open a TCP connection with the given IP address data.
* @note If the function is called without a data struct, a temporary one will be created.
* @param ip the IP address to use
* @param port the port to use
*
* @returns the opened socket
*/
TCPsocket tcp_open(const std::string& ip, int port) {
	IPaddress* ipa = resolve_host(ip, port);
	TCPsocket t = tcp_open(ipa);
	delete ipa;
	return t;
}
/**
* Close the given TCP socket.
* @param tcp the socket to close
*/
void tcp_close(TCPsocket* tcp) {
	SDLNet_TCP_Close(*tcp); // Close the socket
	*tcp = nullptr; // Clear the pointer
}
/**
* Accept a connection on the given socket.
* @param listen the socket to accept a connection on
*
* @returns the accepted socket
*/
TCPsocket tcp_accept(TCPsocket* listen) {
	return SDLNet_TCP_Accept(*listen);
}
/**
* @param tcp the remote connection to return data for
*
* @returns the IP data of the remote TCP connection
*/
IPaddress* get_peer_address(TCPsocket* tcp) {
	return SDLNet_TCP_GetPeerAddress(*tcp);
}
/**
* Send data of a given length via TCP over the given socket.
* @param tcp the socket to send the data through
* @param data the data to send
* @param len the length of the data

* @returns the amount of bytes sent
* @note If the return value is less than @a len, then an error has occured.
*/
int tcp_send(TCPsocket* tcp, const void* data, int len) {
	return SDLNet_TCP_Send(*tcp, data, len);
}
/**
* Send the given string via TCP over the given socket.
* @note If the function is called with a string, it's C-string will be sent.
* @param tcp the socket to send the data through
* @param str the string to send
*
* @returns the amount of bytes sent
* @note If the return value is less than the string length, then an error has occured.
*/
int tcp_send(TCPsocket* tcp, const std::string& str) {
	return tcp_send(tcp, str.c_str(), str.length()+1);
}
/**
* Receive data via TCP from the given socket.
* @param tcp the socket to receive data from
* @param data the pointer to store the data at
* @param maxlen the maximum length of data to receive
*
* @returns the amount of bytes received
* @note If the return value is less than or equal to 0, then an error has occured.
*/
int tcp_recv(TCPsocket* tcp, void* data, int maxlen) {
	return SDLNet_TCP_Recv(*tcp, data, maxlen);
}
/**
* Receive a string via TCP from the given socket.
* @note If the function is called without a pointer location, the data will be returned as a string.
* @param tcp the socket to receive data from
* @param maxlen the maximum length of data to receive
*
* @returns the received string or an empty string on failure
*/
std::string tcp_recv(TCPsocket* tcp, int maxlen) {
	char* data = new char[maxlen]; // Initialize a new character array to temporarily store the data

	if (tcp_recv(tcp, data, maxlen) <= 0) { // Attempt to receive the data
		return "";
	}

	// Convert the data to a string and free the temporary character array
	std::string d (data);
	delete[] data;

	return d;
}

/**
* Open a UDP socket on the given port.
* @note UDP sockets may be used for both sending and receiving data, just not at the same time.
* @note The returned UDPsocket is a pointer typedef.
* @param port the port to use
*
* @returns the opened socket
*/
UDPsocket udp_open(int port) {
	return SDLNet_UDP_Open(port);
}
/**
* Attempt to open multiple UDP sockets on the given port range.
* @note The range that will be opened is [port, range)
* @param port the port to start with
* @param range the range to open
*
* @returns a pair of the last successful port and socket
*/
std::pair<int,UDPsocket> udp_open_range(int port, size_t range) {
	UDPsocket udp = nullptr;
	size_t i;
	for (i=0; (udp == nullptr) && i<range; ++i) {
		udp = udp_open(port+i);
	}
	return std::make_pair(port+i, udp);
}
/**
* Close the given UDP socket.
* @param udp the socket to close
*/
void udp_close(UDPsocket* udp) {
	SDLNet_UDP_Close(*udp); // Close the socket
	*udp = nullptr; // Clear the pointer
}
/**
* Close and reopen the given UDP socket on the given port.
* @param udp the socket to reuse
* @param port the port to use
*/
void udp_reopen(UDPsocket* udp, int port) {
	// Close the socket
	if (*udp != nullptr) {
		udp_close(udp);
	}

	*udp = udp_open(port); // Open the socket on the given port
}
/**
* Bind the given socket to the given IP address data using the given channel.
* @param udp the socket to bind
* @param channel the channel to use, use -1 for the first available channel
* @param ipa the IP address data
*/
int udp_bind(UDPsocket* udp, int channel, IPaddress* ipa) {
	return SDLNet_UDP_Bind(*udp, channel, ipa);
}
/**
* Bind the given socket to the given IP address data using the given channel.
* @note If the function is called with a string and port number, the socket will be bound to the resolved host.
* @param udp the socket to bind
* @param channel the channel to use
* @param ip the IP address to bind to
* @param port the port to use
*
* @returns the bound channel
* @note A return value of -1 means it failed to bind to the channel.
*/
int udp_bind(UDPsocket* udp, int channel, const std::string& ip, int port) {
	IPaddress* ipa = resolve_host(ip, port);
	if (ipa == nullptr) {
		return -1;
	}

	int c = udp_bind(udp, channel, ipa);
	delete ipa;

	return c;
}
/**
* Unbind the given socket from the given channel and close it.
* @param udp the socket to unbind
* @param channel the channel to unbind from
*/
void udp_unbind(UDPsocket* udp, int channel) {
	SDLNet_UDP_Unbind(*udp, channel);
}
/**
* @param udp the remote connection to return data for
* @param channel the channel of the remote connection
*
* @returns the IP data of the remote UDP connection
*/
IPaddress* get_peer_address(UDPsocket udp, int channel) {
	return SDLNet_UDP_GetPeerAddress(udp, channel);
}
/**
* Send packet data via UDP over the given channel.
* @param udp the socket to send the data through
* @param channel the channel to use
* @param packet the data to send
*
* @returns the amount of destinations the data was sent to
*/
int udp_send(UDPsocket udp, int channel, UDPpacket* packet) {
	return SDLNet_UDP_Send(udp, channel, packet);
}
/**
* Send data via UDP over the given channel.
* @note When the function is called without a packet, a temporary one will be constructed.
* @param udp the socket to send the data through
* @param channel the channel to use
* @param data_size the size of the data
* @param data the data to send as a packet
*
* @returns the amount of destinations the data was sent to
*/
int udp_send(UDPsocket udp, int channel, size_t data_size, const Uint8* data) {
	UDPpacket* d = packet_alloc(data_size); // Allocate space for the data packet
	memcpy(d->data, data, data_size); // Set the packet data
	d->len = data_size;

	int r = udp_send(udp, channel, d); // Send the packet

	packet_free(d); // Free the packet

	return r;
}
/**
* Receive packet data via UDP from the given socket.
* @param udp the socket to receive data from
* @param packet the pointer to store that data at
*
* @retval 1 packet received
* @retval 0 no packet received
* @retval -1 failed to received packet
*/
int udp_recv(UDPsocket udp, UDPpacket* packet) {
	return SDLNet_UDP_Recv(udp, packet);
}
/**
* Send multiple data packets via UDP.
* @param udp the socket to send the data through
* @param packets a pointer to an array of packet data
* @param amount the amount of packets in the array
*
* @returns the amount of destinations the data was sent to
*/
int udp_send_vector(UDPsocket udp, UDPpacket** packets, int amount) {
	return SDLNet_UDP_SendV(udp, packets, amount);
}
/**
* Receive multiple data packets via UDP.
* @param udp the socket to receive the data from
* @param packets the pointer to store the data at
*
* @returns the amount of packets received
* @note If the return value is -1, then an error has occured.
*/
int udp_recv_vector(UDPsocket udp, UDPpacket** packets) {
	return SDLNet_UDP_RecvV(udp, packets);
}

/**
* Allocate space for packet data of a given size.
* @param size the size of packet to allocate
*
* @returns the allocated packet or nullptr on failure
*/
UDPpacket* packet_alloc(int size) {
	UDPpacket* packet = SDLNet_AllocPacket(size); // Attempt to allocate the requested space
	if (packet == nullptr) { // If the packet could not be allocated
		std::cerr << "UTIL NETWORK Failed to allocate UDP packet: " << SDLNet_GetError() << "\n"; // Output an error message
	}

	return packet;
}
/**
* Resize the packet data to the given size.
* @param packet the packet to resize
* @param size the new size of the packet
*
* @returns the resized packet or nullptr on failure
*/
UDPpacket* packet_resize(UDPpacket* packet, int size) {
	int newsize = 0; // Define the resultant size of the packet

	newsize = SDLNet_ResizePacket(packet, size); // Attempt to resize the packet
	if (newsize < size) { // If the packet could not be resized
		std::cerr << "UTIL NETWORK Failed to resize UDP packet: " << SDLNet_GetError() << "\n"; // Output an error message
		return nullptr;
	}

	return packet;
}
/**
* Free the packet data.
* @param packet the packet to free
*/
void packet_free(UDPpacket* packet) {
	SDLNet_FreePacket(packet);
}
/**
* Reallocate space for the packet data.
* @note Generally this function should be used like so:
* @code packet = util::network::packet_realloc(packet, size) @endcode
* @param packet the packet to reallocate
* @param size the new size of the packet
*
* @returns the newly allocated packet
*/
UDPpacket* packet_realloc(UDPpacket* packet, int size) {
	packet_free(packet); // Free the packet
	return packet_alloc(size); // Return the allocated packet
}
/**
* Allocate space for an array of packet data.
* @param amount the amount of packets to allocate in the array
* @param size the size of each packet
*
* @returns a pointer to the array of newly allocated packets
*/
UDPpacket** packet_alloc_vector(int amount, int size) {
	UDPpacket** packets = SDLNet_AllocPacketV(amount, size); // Attempt to allocate space for the packets
	if (packets == nullptr) { // If the packets could not be allocated
		std::cerr << "UTIL NETWORK Failed to allocate UDP vector packets: " << SDLNet_GetError() << "\n"; // Output an error message
	}

	return packets;
}
/**
* Free the packet array data.
* @param packets the packet array to free
*/
void packet_free_vector(UDPpacket** packets) {
	SDLNet_FreePacketV(packets);
}

}}

#endif // _BEE_UTIL_NETWORKING
