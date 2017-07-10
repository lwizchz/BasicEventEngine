/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_STRING_H
#define BEE_UTIL_STRING_H 1

#include <string> // Include the required library headers
#include <map>
#include <vector>

#include <SDL2/SDL.h> // Include the SDL2 headers for clipboard access and the Uint8 type

std::string chr(int);
int ord(char);
int ord(const std::string&);
std::string chra(size_t, Uint8*);
std::string chra(std::pair<size_t,Uint8*>);
std::pair<size_t,Uint8*> orda(const std::string&);
std::string string_lower(const std::string&);
std::string string_upper(const std::string&);
std::string string_letters(const std::string&);
std::string string_digits(const std::string&);
std::string string_lettersdigits(const std::string&);
std::map<int,std::string> split(const std::string&, char, bool);
std::map<int,std::string> split(const std::string&, char);
std::map<int,std::string> handle_newlines(const std::string&);
std::vector<std::string> splitv(const std::string&, char, bool);
std::string join(const std::map<int,std::string>&, char);
std::string joinv(const std::vector<std::string>&, char);
std::string ltrim(const std::string&);
std::string rtrim(const std::string&);
std::string trim(const std::string&);
bool stringtobool(const std::string&);
std::string booltostring(bool);
std::string string_replace(const std::string&, const std::string&, const std::string&);
std::string string_escape(const std::string&);
std::string string_unescape(const std::string&);
std::string string_repeat(size_t, const std::string&);
std::string string_tabulate(const std::vector<std::vector<std::string>>&);
bool clipboard_has_text();
std::string clipboard_get_text();
int clipboard_set_text(const std::string&);

#endif // BEE_UTIL_STRING_H
