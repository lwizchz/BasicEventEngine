/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
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
#include <map>
#include <SDL2/SDL.h>

std::string chr(int c) {
        char character = c;
        return std::to_string(character);
}

int ord(char c) {
        std::string str = std::to_string(c);
        return str[0];
}
int ord(std::string s) {
        return s[0];
}
std::string chra(Uint8* carray) {
        std::string str;
        for (unsigned int i=0; i<carray[0]; i++) {
                str += chr(carray[i+1]);
        }
        return str;
}
Uint8* orda(std::string s) {
        Uint8* carray = (Uint8*)malloc(s.length()+1);
        carray[0] = s.length()+1;
        for (unsigned int i=0; i<s.length(); i++) {
                carray[i+1] = s[i];
        }
        return carray;
}

std::string string_lower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
}
std::string string_upper(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
}

std::string string_letters(std::string str) {
        return std::regex_replace(str, std::regex("[^[:alnum:]]"), "");
}
std::string string_digits(std::string str) {
        return std::regex_replace(str, std::regex("[^[:digit:]]"), "");
}
std::string string_lettersdigits(std::string str) {
        return std::regex_replace(str, std::regex("[^[:alnum:][:digit:]]"), "");
}

std::map<int,std::string> split(std::string input, char delimiter) {
        std::map<int,std::string> output;
	if (input.size() > 0) {
		std::istringstream input_stream (input);
		while (!input_stream.eof()) {
			std::string tmp;
			getline(input_stream, tmp, delimiter);
			output.insert(std::make_pair(output.size(), tmp));
		}
	}
	return output;
}
std::map<int,std::string> handle_newlines(std::string input) {
	return split(input, '\n');
}

std::string ltrim(std::string str) {
        unsigned int i;
        for (i=0; i<str.length(); i++) {
                if (!isspace(str[i])) {
                        break;
                }
        }
        return str.substr(i);
}
std::string rtrim(std::string str) {
        int i;
        for (i=str.length()-1; i>=0; i--) {
                if (!isspace(str[i])) {
                        break;
                }
        }
        return str.substr(0, i+1);
}
std::string trim(std::string str) {
        return rtrim(ltrim(str));
}

bool stringtobool(std::string str) {
        str = string_lower(trim(str));
        std::vector<std::string> false_values = {"0", "false"};
        for (auto& s : false_values) {
                if (str == s) {
                        return false;
                }
        }
        return true;
}
std::string booltostring(bool b) {
        if (b) {
                return "true";
        }
        return "false";
}

std::string string_replace(std::string str, std::string search, std::string replacement) {
        size_t start_pos = 0;
        while ((start_pos = str.find(search, start_pos)) != std::string::npos) {
                str.replace(start_pos, search.length(), replacement);
                start_pos += replacement.length();
        }
        return str;
}

bool clipboard_has_text() {
        return (SDL_HasClipboardText() == SDL_TRUE) ? true : false;
}
std::string clipboard_get_text() {
        char* cstr = SDL_GetClipboardText();
        std::string s (cstr);
        free(cstr);
        return s;
}
int clipboard_set_text(std::string str) {
        return SDL_SetClipboardText(str.c_str());
}

#endif // _BEE_UTIL_STRING_H
