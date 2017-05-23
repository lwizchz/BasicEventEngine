/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_PLATFORM_H
#define _BEE_UTIL_PLATFORM_H 1

#include <sys/stat.h> // Include the required file functions
#include <string>

#ifdef __linux__

#include <iostream>
#include <string.h>

#include <sys/time.h> // Include the required functions for non-blocking commandline input
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h> // Include the required network functions
#include <linux/limits.h> // Include the required PATH_MAX

#elif _WIN32

#include <ws2tcpip.h> // Include the required Windows headers
#include <winsock2.h>
#include <windows.h>
#include <direct.h>
#include <conio.h> // Include the required functions for non-blocking commandline input

#endif // _WIN32

int bee_get_platform();
std::string bee_get_path();
std::string bee_itos(int);
int bee_stoi(const std::string&);
time_t bee_inc_dst(time_t);
bool bee_has_commandline_input();
int bee_remove(const std::string&);
bool bee_dir_exists(const std::string&);
int bee_mkdir(const char*, mode_t);
std::string bee_mkdtemp(const std::string&);
std::string bee_inet_ntop(const void* src);
int bee_commandline_color(int);
int bee_commandline_color_reset();

#endif // _BEE_UTIL_PLATFORM_H
