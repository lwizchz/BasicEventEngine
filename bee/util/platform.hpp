/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_PLATFORM_H
#define _BEE_UTIL_PLATFORM_H 1

// Platform compatibility functions

#include <sys/stat.h>

int bee_mkdir(const char*, mode_t);
std::string bee_mkdtemp(char*);

std::string bee_inet_ntop(const void* src);

#ifdef _WINDOWS

#include <ws2tcpip.h>
#include <winsock2.h>
#include <windows.h>

int bee_mkdir(const char* path, mode_t mode) {
        return mkdir(path);
}
std::string bee_mkdtemp(char*) {
        static char path[MAX_PATH];
        if (path[0] == '\0') {
                GetTempPath(MAX_PATH, path);
        }
        return path;
}

std::string bee_inet_ntop(const void* src) {
        char dest[INET_ADDRSTRLEN];
        //InetNtop(AF_INET, src, dest, INET_ADDRSTRLEN); // FIXME: it won't let me include the Winsock2 library, ws2_32.lib
        return std::string(dest);
}

#else

#include <arpa/inet.h>

int bee_mkdir(const char* path, mode_t mode) {
        return mkdir(path, mode);
}
std::string bee_mkdtemp(char* t) {
        std::string path (mkdtemp(t));
        return path;
}

std::string bee_inet_ntop(const void* src) {
        char dest[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, src, dest, INET_ADDRSTRLEN);
        return std::string(dest);
}

#endif

#endif // _BEE_UTIL_PLATFORM_H
