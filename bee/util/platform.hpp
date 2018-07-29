/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_PLATFORM_H
#define BEE_UTIL_PLATFORM_H 1

#include <string>

#ifdef _WIN32
	typedef int mode_t;
#endif

namespace util { namespace platform {

int get_platform();

std::string get_path();

int remove(const std::string&);
int dir_exists(const std::string&);
int mkdir(const std::string&, mode_t);
std::string mkdtemp(const std::string&);

std::string inet_ntop(const void* src);

bool has_commandline_input();
void commandline_color(std::ostream*, int);
void commandline_color_reset(std::ostream*);
void commandline_clear();

}}

#endif // BEE_UTIL_PLATFORM_H
