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

#include <string> // Include the required library headers
#include <algorithm>
#include <regex>

#include <SDL2/SDL.h> // Include the SDL2 headers for clipboard access

/*
* chr() - Convert the given character code to a string
* @c: the character code to convert
*/
std::string chr(int c) {
        std::string str = "0";
        str[0] = c;
        return str;
}
/*
* ord() - Convert the given character to a character code
* @c: the character to convert
*/
int ord(char c) {
        return (int)c;
}
/*
* ord() - Convert the given string's first character into a character code
* @s: the string to operate on
*/
int ord(const std::string& s) {
        return s[0];
}
/*
* chra() - Convert the given array of character data into a string
* ! Note that the first element must be the total length of the array
* @carray: the array to convert
*/
std::string chra(Uint8* carray) {
        std::string str = ""; // Initialize an empty string to store the data in
        for (unsigned int i=0; i<carray[0]; i++) { // Iterate over the array and append each element as a character
                str.append(chr(carray[i+1]));
        }
        return str; // Return the string on success
}
/*
* orda() - Convert the given string into an array of character data
* ! The array format is the same as accepted by chra()
* ! Be sure to free the character array when you are done using it
* @s: the string to convert
*/
Uint8* orda(const std::string& s) {
        //Uint8* carray = (Uint8*)malloc(s.length()+1); // Allocate space for the string and the metadata
        Uint8* carray = new Uint8[s.length()+1]; // Allocate space for the string and the metadata
        carray[0] = s.length(); // Prepend the length of the string
        for (unsigned int i=0; i<s.length(); i++) { // Iterate over the string and add each character to the array
                carray[i+1] = s[i];
        }
        return carray; // Return the array on success
}

/*
* string_lower() - Change the characters in the string so that they are all lowercase
* @str: the string to operate on
*/
std::string string_lower(const std::string& str) {
        std::string s = ""; // Declare a new string of the same size as the given string
        s.resize(str.length());
        std::transform(str.begin(), str.end(), s.begin(), ::tolower); // Transform the given string to lowercase
        return s;
}
/*
* string_upper() - Change the characters in the string so that they are all uppercase
* @str: the string to operate on
*/
std::string string_upper(const std::string& str) {
        std::string s = ""; // Declare a new string of the same size as the given string
        s.resize(str.length());
        std::transform(str.begin(), str.end(), s.begin(), ::toupper); // Transform the given string to uppercase
        return s;
}
/*
* string_letters() - Filter the characters in the string so that only alphabetical characters are returned
* @str: the string to operate on
*/
std::string string_letters(const std::string& str) {
        return std::regex_replace(str, std::regex("[^[:alpha:]]"), std::string(""));
}
/*
* string_letters() - Filter the characters in the string so that only digits are returned
* @str: the string to operate on
*/
std::string string_digits(const std::string& str) {
        return std::regex_replace(str, std::regex("[^[:digit:]]"), std::string(""));
}
/*
* string_letters() - Filter the characters in the string so that only alphanumeric characters are returned
* @str: the string to operate on
*/
std::string string_lettersdigits(const std::string& str) {
        return std::regex_replace(str, std::regex("[^[:alnum:]]"), std::string(""));
}

/*
* split() - Split a string by a given delimiter and return the data as a map
* @input: the string to operate on
* @delimiter: the character to use to split the string
*/
std::map<int,std::string> split(const std::string& input, char delimiter) {
        std::map<int,std::string> output; // Declare a map to store the split strings
	if (!input.empty()) { // Only attempt to operate if the input is not empty
		std::istringstream input_stream (input); // Convert the string to a string stream for iteration
		while (!input_stream.eof()) { // While there is more data in the string stream
			std::string tmp;
			getline(input_stream, tmp, delimiter); // Fetch the next substring before the given delimiter and store it in the temporary string
			output.insert(std::make_pair(output.size(), tmp)); // Insert the substring into the map
		}
	}
	return output; // Return the map on success
}
/*
* handle_newlines() - Split a string by newlines and return the data as a map
* @input: the string to operate on
*/
std::map<int,std::string> handle_newlines(const std::string& input) {
	return split(input, '\n');
}

/*
* ltrim() - Trim the string for whitespace on the left side
* @str: the string to operate on
*/
std::string ltrim(const std::string& str) {
        unsigned int i;
        for (i=0; i<str.length(); i++) { // Iterate over the given string until there is a non-space character
                if (!isspace(str[i])) {
                        break;
                }
        }
        return str.substr(i); // Return the string beginning at the first non-space character
}
/*
* rtrim() - Trim the string for whitespace on the right side
* @str: the string to operate on
*/
std::string rtrim(const std::string& str) {
        int i;
        for (i=str.length()-1; i>=0; i--) { // Reverse iterate over the given string until there is a non-space character
                if (!isspace(str[i])) {
                        break;
                }
        }
        return str.substr(0, i+1); // Return the string ending at the first non-space character
}
/*
* trim() - Trim the string for whitespace on both sides
* @str: the string to operate on
*/
std::string trim(const std::string& str) {
        return rtrim(ltrim(str));
}

/*
* stringtobool() - Convert the given string to a boolean value
* ! Currently everything besides "0" and "false" will evaluate to true
* @str: the string to convert
*/
bool stringtobool(const std::string& str) {
        std::string lstr = string_lower(trim(str)); // Trim the string and convert it to lowercase
        std::vector<std::string> false_values = {"0", "false"}; // Declare the values which shall evaluate to false
        for (auto& s : false_values) { // Iterate over the false values and compare them to the string
                if (lstr == s) {
                        return false; // Return false if the string is one of the false values
                }
        }
        return true; // Return true if the string is not any of the false values
}
/*
* booltostring() - Convert the given boolean value to a string
* @b: the bool to convert
*/
std::string booltostring(bool b) {
        if (b) {
                return "true"; // Return "true" if true
        }
        return "false"; // Otherwise, return false
}

/*
* string_replace() - Replace all occurences of the given search string with the given replacement
* @str: the string to operate on
* @search: the string to search and replace
* @replacment: the string to replace the search
*/
std::string string_replace(const std::string& str, const std::string& search, const std::string& replacement) {
        std::string s (str); // Create a new string so that the given string is not modified
        size_t start_pos = 0; // Start at the beginning of the string
        while ((start_pos = s.find(search, start_pos)) != std::string::npos) { // Continue to search through the string for the given substring
                s.replace(start_pos, search.length(), replacement); // Replace the substring
                start_pos += replacement.length(); // Start from the end of the replacement
        }
        return s; // Return the modified string
}

/*
* clipboard_has_text() - Return whether there is currently text in the clipboard
*/
bool clipboard_has_text() {
        return (SDL_HasClipboardText() == SDL_TRUE) ? true : false;
}
/*
* clipboard_get_text() - Return the text that is currently in the clipboard
*/
std::string clipboard_get_text() {
        char* cstr = SDL_GetClipboardText(); // Get the clipboard text and convert it to a string
        std::string s (cstr);
        free(cstr);
        return s; // Return the string on success
}
/*
* clipboard_set_text() - Copy the given text to the clipboard
* ! See https://wiki.libsdl.org/SDL_SetClipboardText for details
* @str: the string to copy
*/
int clipboard_set_text(const std::string& str) {
        return SDL_SetClipboardText(str.c_str());
}

#endif // _BEE_UTIL_STRING_H
