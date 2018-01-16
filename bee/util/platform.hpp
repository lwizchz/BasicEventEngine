/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_PLATFORM_H
#define BEE_UTIL_PLATFORM_H 1

#include <string>

#ifdef _WIN32
	typedef int mode_t;
#endif

int bee_get_platform();

std::string bee_get_path();

std::string bee_itos(int);
int bee_stoi(const std::string&);

int bee_remove(const std::string&);
int bee_dir_exists(const std::string&);
int bee_mkdir(const std::string&, mode_t);
std::string bee_mkdtemp(const std::string&);

std::string bee_inet_ntop(const void* src);

bool bee_has_commandline_input();
int bee_commandline_color(std::ostream*, int);
int bee_commandline_color_reset(std::ostream*);
int bee_commandline_clear();

#endif // BEE_UTIL_PLATFORM_H
