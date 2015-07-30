/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_STRING_H
#define _BEE_UTIL_STRING_H 1

// String handling functions

#include <string>
#include <algorithm>
#include <regex>

std::string chr(int c) {
        char character = c;
        return std::to_string(character);
}

template <typename T>
int ord(T s) {
        std::string str = std::to_string(s);
        return str[0];
}

std::string string_lower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
}
std::string string_upper(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
}

std::string string_letters(std::string str){
        return std::regex_replace(str, std::regex("[^[:alnum:]]"), "");
}
std::string string_digits(std::string str){
        return std::regex_replace(str, std::regex("[^[:digit:]]"), "");
}
std::string string_lettersdigits(std::string str){
        return std::regex_replace(str, std::regex("[^[:alnum:][:digit:]]"), "");
}

bool clipboard_has_text() {
        std::cerr << "Clipboard not yet implemented\n";
        return false;
}
std::string clipboard_get_text() {
        std::cerr << "Clipboard not yet implemented\n";
        return "";
}
int clipboard_set_text(std::string str) {
        std::cerr << "Clipboard not yet implemented\n";
        return 1;
}

#endif // _BEE_UTIL_STRING_H
