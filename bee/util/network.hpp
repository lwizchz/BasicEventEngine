/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_NETWORK_H
#define _BEE_UTIL_NETWORK_H 1

// Networking functions

#include <string>
#include <SDL2/SDL_net.h>

int network_init() {
        int r = SDLNet_Init();
        if (r) {
                std::cerr << "Error initializing network functionality: " << SDLNet_GetError() << "\n";
        }
        return r;
}
int network_quit() {
        SDLNet_Quit();
        return 0;
}
IPaddress* network_get_peer_address(TCPsocket tcp) {
        return SDLNet_TCP_GetPeerAddress(tcp);
}

TCPsocket network_tcp_open(std::string ip, int port) {
        IPaddress ipa;
        TCPsocket tcp;

        if (ip.empty()) { // Host
                if (SDLNet_ResolveHost(&ipa, NULL, port)) {
                        std::cerr << "Failed to resolve host: " << SDLNet_GetError() << "\n";
                        return NULL;
                }

                tcp = SDLNet_TCP_Open(&ipa);
                if (!tcp) {
                        std::cerr << "Failed to host TCP on port " << port << ": " << SDLNet_GetError();
                        return NULL;
                }
        } else { // Client
                if (SDLNet_ResolveHost(&ipa, ip.c_str(), port)) {
                        std::cerr << "Failed to resolve host: " << SDLNet_GetError() << "\n";
                        return NULL;
                }

                tcp = SDLNet_TCP_Open(&ipa);
                if (!tcp) {
                        std::cerr << "Failed to connect via TCP to \"" << ip << ":" << port << "\": " << SDLNet_GetError();
                        return NULL;
                }
        }

        return tcp;
}
int network_tcp_close(TCPsocket tcp) {
        SDLNet_TCP_Close(tcp);
        return 0;
}
TCPsocket network_tcp_accept(TCPsocket listen) {
        return SDLNet_TCP_Accept(listen);
}
int network_tcp_send(TCPsocket tcp, const void* data, int len) {
        int r = SDLNet_TCP_Send(tcp, data, len);
        if (r < len) {
                std::cerr << "Failed to send full data over TCP: " << SDLNet_GetError() << "\n";
        }
        return r;
}
int network_tcp_send(TCPsocket tcp, std::string str) {
        return network_tcp_send(tcp, str.c_str(), str.length()+1);
}
int network_tcp_recv(TCPsocket tcp, void* data, int maxlen) {
        int r = SDLNet_TCP_Recv(tcp, data, maxlen);
        if (r <= 0) {
                std::cerr << "Failed to recieve data over TCP: " << SDLNet_GetError() << "\n";
        }
        return r;
}
std::string network_tcp_recv(TCPsocket tcp, int maxlen) {
        char data[maxlen];
        network_tcp_recv(tcp, data, maxlen);
        return std::string(data);
}

#endif // _BEE_UTIL_NETWORK_H
