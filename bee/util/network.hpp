/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_NETWORK_H
#define _BEE_UTIL_NETWORK_H 1

// Networking functions

#include <string> // Include the required library headers

#include <SDL2/SDL_net.h> // Include the SDL2 networking headers

/*
* network_init() - Initialize SDL's networking functionality
*/
int network_init() {
        int r = SDLNet_Init(); // Attempt to initiliaze networking
        if (r != 0) { // If an error was returned, print it out
                std::cerr << "Error initializing network functionality: " << SDLNet_GetError() << "\n";
        }
        return r; // Return the error code
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
* ! Note that it is the user's
* @ip: the IP address to resolve
* @port: the port the use for the host
*/
IPaddress* network_resolve_host(const std::string& ip, int port) {
        IPaddress* ipa = new IPaddress(); // Allocate a new IPaddress struct

        if (ip.empty()) { // Resolve the data for a user acting as a host
                if (SDLNet_ResolveHost(ipa, nullptr, port)) { // Attempt to fill the struct
                        std::cerr << "Failed to resolve host: " << SDLNet_GetError() << "\n"; // Output the error message
                        return nullptr; // Return nullptr on failure
                }
        } else { // Resolve the data for a user connecting to a host
                if (SDLNet_ResolveHost(ipa, ip.c_str(), port)) { // Attempt to fill the struct
                        std::cerr << "Failed to resolve host: " << SDLNet_GetError() << "\n"; // Output the error message
                        return nullptr; // Return nullptr on failure
                }
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
        TCPsocket tcp; // Declare a socket data struct

        tcp = SDLNet_TCP_Open(ipa); // Attempt to open the socket
        if (tcp == nullptr) { // If the socket failed to open
                std::cerr << "Failed to open TCP on port " << network_get_port(ipa->port) << ": " << SDLNet_GetError() << "\n"; // Output the error message
                return nullptr; // Return nullptr on failure
        }

        return tcp; // Return the socket on success
}
/*
* network_tcp_open() - Open a TCP connection with the given IP address data
* ! When the function is called without a data struct, simply call it with a temporary one
* @ip: the IP address to use
* @port: the port to use
*/
TCPsocket network_tcp_open(const std::string& ip, int port) {
        return network_tcp_open(network_resolve_host(ip, port));
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
        IPaddress* ipa = nullptr; // Declare an IP data struct

        ipa = SDLNet_TCP_GetPeerAddress(*tcp); // Attempt to get the peer address info
        if (ipa == nullptr) { // If the information could not be retrieved
                std::cerr << "Failed to get TCP peer address: " << SDLNet_GetError() << "\n"; // Output the error message
        }

        return ipa; // Return the IP data on success
}
/*
* network_tcp_send() - Send data of a given length via TCP over the given socket
* @tcp: the socket to send the data through
* @data: the data to send
* @len: the length of the data
*/
int network_tcp_send(TCPsocket* tcp, const void* data, int len) {
        int r = SDLNet_TCP_Send(*tcp, data, len); // Attempt to send the data

        if (r < len) { // If less data was sent than requested
                std::cerr << "Failed to send full data over TCP: " << SDLNet_GetError() << "\n"; // Output the error message
        }

        return r; // Return the length of data sent
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
        int r = SDLNet_TCP_Recv(*tcp, data, maxlen); // Attempt to receive the data

        if (r <= 0) { // If no data was received or another error occured
                std::cerr << "Failed to receive data over TCP: " << SDLNet_GetError() << "\n"; // Output the error message
        }

        return r; // Return the amount of data received on success, or the error code on failure
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
        UDPsocket udp = nullptr; // Declare a socket data struct

        udp = SDLNet_UDP_Open(port); // Attempt to open the socket
        if (udp == nullptr) { // If the socket failed to open
                std::cerr << "Failed to open UDP on port " << port << ": " << SDLNet_GetError() << "\n"; // Output the error message
                return nullptr; // Return nullptr on failure
        }

        return udp; // Return the socket on success
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
        network_udp_reopen(udp, network_get_port(ipa->port)); // Reset the socket in case it was previously bound

        int c = SDLNet_UDP_Bind(*udp, channel, ipa); // Attempt to bind the socket to the given IP address
        if (c == -1) { // If the socket failed to bind
                std::cerr << "Failed to bind UDP on channel " << channel << ": " << SDLNet_GetError() << "\n"; // Output the error message
        }

        return c; // Return the channel on success, or -1 on failure
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
        return network_udp_bind(udp, channel, network_resolve_host(ip, port));
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
        network_udp_close(udp); // Close the socket
        return 0; // Return 0 on success
}
/*
* network_get_peer_address() - Return the IP data of the remote UDP connection
* @udp: the remote connection to return data for
* @channel: the channel of the remote connection
*/
IPaddress* network_get_peer_address(UDPsocket udp, int channel) {
        IPaddress* ipa = nullptr; // Declare an IP data struct

        ipa = SDLNet_UDP_GetPeerAddress(udp, channel); // Attempt to get the peer address info
        if (ipa == nullptr) { // If the information could not be retrieved
                std::cerr << "Failed to get UDP peer address: " << SDLNet_GetError() << "\n"; // Output the error message
        }

        return ipa; // Return the IP data on success
}
/*
* network_udp_send() - Send packet data via UDP over the given channel
* @udp: the socket to send the data through
* @channel: the channel to use
* @packet: the data to send
*/
int network_udp_send(UDPsocket udp, int channel, UDPpacket* packet) {
        int sent = 0; // Define the amount of destinations the data was sent to

        sent = SDLNet_UDP_Send(udp, channel, packet); // Attempt to send the data
        if (sent == 0) { // If the data could not be sent to anyone
                std::cerr << "Failed to send packet over UDP: " << SDLNet_GetError() << "\n"; // Output the error message
                return -1; // Return -1 on failure
        }

        return sent; // Return the amount of destinations that data was sent to on success
}
/*
* network_udp_send() - Send data via UDP over the given channel
* ! When the function is called without a packet, simply call it with a temporary one
* @udp: the socket to send the data through
* @channel: the channel to use
* @data: the data to send as a packet
*/
int network_udp_send(UDPsocket udp, int channel, Uint8* data) {
	UDPpacket* d = network_packet_alloc(data[0]); // Allocate space for the data packet
	d->data = data; // Set the packet data

        int r = network_udp_send(udp, channel, d); // Send the packet

        network_packet_free(d); // Free the packet

        return r; // Return the amount of destinations the data was sent to on success, or -1 on failure
}
/*
* network_udp_send() - Send formatted data via UDP over the given channel
* ! When the function is called with specific data, simply call it after properly formatting the data
* ! See bee/game/network.cpp for formatting details
* @udp: the socket to send the data through
* @channel: the channel to use
* @id: the sender's id number
* @signal: the primary message signal
* @data: the extra data to send
*/
int network_udp_send(UDPsocket udp, int channel, Uint8 id, Uint8 signal, Uint8 data) {
        Uint8 d[] = {3, id, signal, data}; // Format the data according to bee/game/network.cpp
        return network_udp_send(udp, channel, d); // Send the data and return the sent destinations on success, or -1 on failure
}
/*
* network_udp_recv() - Receive packet data via UDP from the given socket
* @udp: the socket to receive data from
* @packet: the pointer to store that data at
*/
int network_udp_recv(UDPsocket udp, UDPpacket* packet) {
        int recv = 0; // Define whether data was received or not

        recv = SDLNet_UDP_Recv(udp, packet); // Attempt to receive the data
        if (recv == -1) { // If an error occured while receiving the data
                std::cerr << "Failed to receive data over UDP: " << SDLNet_GetError() << "\n"; // Output the error message
                return -1; // Return -1 on failure
        }

        return recv; // Return whether a packet was received: 1 on success, 0 for no packet
}
/*
* network_udp_sendv() - Send multiple data packets via UDP
* @udp: the socket to send the data through
* @packets: a pointer to an array of packet data
* @amount: the amount of packets in the array
*/
int network_udp_send_vector(UDPsocket udp, UDPpacket** packets, int amount) {
        int sent = 0; // Define how many total packets were sent to each destination

        sent = SDLNet_UDP_SendV(udp, packets, amount); // Attempt to send the data
        if (sent == 0) { // If none of the data could be sent to anyone
                std::cerr << "Failed to send vector data over UDP: " << SDLNet_GetError() << "\n"; // Output the error message
                return 0; // Return 0 on failure
        }

        return sent; // Return how many packets were sent in total on success
}
/*
* network_udp_recvv() - Receive multiple data packets via UDP
* @udp: the socket to receive the data from
* @packets: the pointer to store the data at
*/
int network_udp_recv_vector(UDPsocket udp, UDPpacket** packets) {
        int recv = 0; // Define how many packets were received

        recv = SDLNet_UDP_RecvV(udp, packets); // Attempt to receive the data
        if (recv == -1) { // If an error occured while receiving the data
                std::cerr << "Failed to receive vector data over UDP: " << SDLNet_GetError() << "\n"; // Output the error message
                return -1; // Return -1 on failure
        }

        return recv; // Return the amount of packets that were received on success, or 0 for no packets
}

/*
* network_packet_alloc() - Allocate space for packet data of a given size
* @size: the size of packet to allocate
*/
UDPpacket* network_packet_alloc(int size) {
        UDPpacket* packet = nullptr; // Declare a packet data pointer

        packet = SDLNet_AllocPacket(size); // Attempt to allocate the requested space
        if (packet == nullptr) { // If the packet could not be allocated
                std::cerr << "Failed to allocate UDP packet: " << SDLNet_GetError() << "\n"; // Output the error message
                return nullptr; // Return nullptr on failure
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
                std::cerr << "Failed to resize UDP packet: " << SDLNet_GetError() << "\n"; // Output the error message
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
        packet = nullptr; // Clear the pointer
        return 0; // Return 0 on success
}
/*
* network_packet_realloc() - Reallocate space for the packet data
* ! Perhaps use this function if network_packet_resize() fails
* @packet: the packet to reallocate
* @size: the new size of the packet
*/
int network_packet_realloc(UDPpacket* packet, int size) {
        network_packet_free(packet); // Free the packet
        packet = network_packet_alloc(size); // Allocate the packet
        return (packet == nullptr) ? 1 : 0; // Return 0 on success and 1 on failure
}
/*
* network_packet_alloc_vector() - Allocate space for an array of packet data
* @amount: the amount of packets to allocate in the array
* @size: the size of each packet
*/
UDPpacket** network_packet_alloc_vector(int amount, int size) {
        UDPpacket** packets = nullptr; // Declare a packet array pointer

        packets = SDLNet_AllocPacketV(amount, size); // Attempt to allocate space for the packets
        if (packets == nullptr) { // If the packets could not be allocated
                std::cerr << "Failed to allocate UDP vector packets: " << SDLNet_GetError() << "\n"; // Output the error message
                return nullptr; // Return nullptr on failure
        }

        return packets; // Return the packet array pointer on success
}
/*
* network_packet_free_vector() - Free the packet array data
* @packets: the packet array to free
*/
int network_packet_free_vector(UDPpacket** packets) {
        SDLNet_FreePacketV(packets); // Free the packets
        packets = nullptr; // Clear the pointer
        return 0; // Return 0 on success
}

#endif // _BEE_UTIL_NETWORK_H
