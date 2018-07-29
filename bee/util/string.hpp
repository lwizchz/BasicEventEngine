/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_STRING_H
#define BEE_UTIL_STRING_H 1

#include <string> // Include the required library headers
#include <map>
#include <vector>

#include <SDL2/SDL.h> // Include the SDL2 headers for clipboard access and the Uint8 type

namespace util {

std::string chr(int);
std::string chra(size_t, const Uint8*);
std::string chra(const std::vector<Uint8>&);
std::vector<Uint8> orda(const std::string&);

std::vector<std::string> splitv(const std::string&, char, bool);
std::string joinv(const std::vector<std::string>&, char);

std::string ltrim(const std::string&);
std::string rtrim(const std::string&);
std::string trim(const std::string&);

bool clipboard_has_text();
std::string clipboard_get_text();
int clipboard_set_text(const std::string&);

namespace string {
	std::string lower(const std::string&);
	std::string upper(const std::string&);
	std::string title(const std::string&);
	std::string letters(const std::string&);
	std::string digits(const std::string&);
	std::string lettersdigits(const std::string&);

	bool tobool(const std::string&);
	std::string frombool(bool);

	std::string replace(const std::string&, const std::string&, const std::string&);
	std::string escape(const std::string&);
	std::string unescape(const std::string&);
	std::string repeat(size_t, const std::string&);

	std::string tabulate(const std::vector<std::vector<std::string>>&);

	bool is_floating(const std::string&);
	bool is_integer(const std::string&);
}}

#endif // BEE_UTIL_STRING_H
