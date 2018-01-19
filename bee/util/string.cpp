/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_STRING
#define BEE_UTIL_STRING 1

// String handling functions

#include <algorithm>
#include <regex>

#include "string.hpp" // Include the function definitions

namespace util {

/**
* Convert the given character code to a string.
* @param c the character code to convert
*
* @returns the corresponding character as a string
*/
std::string chr(int c) {
	return std::string(1, c);
}
/**
* Convert the given array of character data into a string.
* @param size the size of the array
* @param array the array to convert
*
* @returns the corresponding string
*/
std::string chra(size_t size, const Uint8* array) {
	std::string str = "";
	for (size_t i=0; i<size; i++) { // Iterate over the array and append each element as a character
		str.append(chr(array[i]));
	}
	return str;
}
/**
* Convert the given array of character data into a string.
* @param array the array to convert
*
* @returns the corresponding string
*/
std::string chra(const std::vector<Uint8>& array) {
	return chra(array.size(), array.data());
}
/**
* Convert the given string into an array of character data.
* @param s the string to convert
*
* @returns the corresponding character array
*/
std::vector<Uint8> orda(const std::string& s) {
	std::vector<Uint8> v;
	for (size_t i=0; i<s.length(); i++) { // Iterate over the string and add each character to the array
		v.push_back(s[i]);
	}
	return v;
}

/**
* Split a string by a given delimiter.
* @param input the string to operate on
* @param delimiter the character to use to split the string
* @param should_respect_containers whether quotes and other containers should split the string
*
* @returns the vector containing the tokens of the given string
*/
std::vector<std::string> splitv(const std::string& input, char delimiter, bool should_respect_containers) {
	std::vector<std::string> output;
	std::string cont_start = "\"[{"; // The valid containers: double quotes, square brackets, and curly braces
	std::string cont_end = "\"]}";
	std::vector<size_t> containers;

	size_t token_start = 0; // Store the position of the beginning of each token
	for (size_t i=0; i<input.length(); ++i) { // Iterate over each character in the string
		char c = input[i]; // Get the current character

		if (c == delimiter) { // If the character is a delimiter, store a substring in the vector
			output.emplace_back(input.substr(token_start, i-token_start));
			token_start = i+1; // Begin the next token after the delimiter
		} else if ((cont_start.find(c) != std::string::npos)&&(should_respect_containers)) { // If the character is a container, handle it separately
			if ((i>0)&&(input[i-1] == '\\')) { // Skip escaped containers
				continue;
			}

			containers.push_back(cont_start.find(c)); // Store the container index

			while (++i < input.length()) { // Iterate over the string until the container or string end is reached
				if ((cont_end.find(input[i]) == containers.back())&&(input[i-1] != '\\')) {
					containers.pop_back();
					if (containers.empty()) {
						break;
					}
				} else if (cont_start.find(input[i]) != std::string::npos) { // Nest the containers if another is found
					containers.push_back(cont_start.find(input[i]));
				}
			}
		}
	}
	if (token_start < input.length()) {  // Add the last token to the vector if it exists
		output.emplace_back(input.substr(token_start, input.length()-token_start));
	}

	return output;
}
/**
* Join a vector of strings by the given delimiter.
* @param input the vector to join together
* @param delimiter the character to join the string with
*
* @returns the combined string
*/
std::string joinv(const std::vector<std::string>& input, char delimiter) {
	std::string output = "";
	for (auto& l : input) { // Iterate over the vector
		output += l; // Add each line to the output
		output += delimiter; // Append the delimiter for each line
	}

	if (output.size() > 0) {
		output.pop_back(); // Remove the last delimiter
	}

	return output;
}

/**
* Trim the string for whitespace on the left side.
* @param str the string to operate on
*
* @returns the trimmed string
*/
std::string ltrim(const std::string& str) {
	unsigned int i;
	for (i=0; i<str.length(); i++) { // Iterate over the given string until there is a non-space character
		if (!isspace(str[i])) {
			break;
		}
	}
	return str.substr(i);
}
/**
* Trim the string for whitespace on the right side.
* @param str the string to operate on
*
* @returns the trimmed string
*/
std::string rtrim(const std::string& str) {
	int i;
	for (i=str.length()-1; i>=0; i--) { // Reverse iterate over the given string until there is a non-space character
		if (!isspace(str[i])) {
			break;
		}
	}
	return str.substr(0, i+1);
}
/**
* Trim the string for whitespace on both sides.
* @param str the string to operate on
*
* @returns the trimmed string
*/
std::string trim(const std::string& str) {
	return rtrim(ltrim(str));
}

/**
* @returns whether there is currently text in the clipboard
*/
bool clipboard_has_text() {
	return (SDL_HasClipboardText() == SDL_TRUE) ? true : false;
}
/**
* @returns the text that is currently in the clipboard
*/
std::string clipboard_get_text() {
	// Get the clipboard text and convert it to a string
	char* cstr = SDL_GetClipboardText();
	std::string s (cstr);

	free(cstr);

	return s;
}
/**
* Copy the given text to the clipboard.
* @see https://wiki.libsdl.org/SDL_SetClipboardText for details
* @param str the string to copy
*
* @retval 0 success
* @retval <0 failed to set the text
*/
int clipboard_set_text(const std::string& str) {
	return SDL_SetClipboardText(str.c_str());
}

/**
* Change the characters in the string so that they are all lowercase.
* @param str the string to operate on
*
* @returns the lowercase string
*/
std::string string::lower(const std::string& str) {
	std::string s = "";
	s.resize(str.length());
	std::transform(str.begin(), str.end(), s.begin(), ::tolower); // Transform the given string to lowercase
	return s;
}
/**
* Change the characters in the string so that they are all uppercase.
* @param str the string to operate on
*
* @returns the uppercase string
*/
std::string string::upper(const std::string& str) {
	std::string s = "";
	s.resize(str.length());
	std::transform(str.begin(), str.end(), s.begin(), ::toupper); // Transform the given string to uppercase
	return s;
}
/**
* Change the characters in the string so that they are titlecase.
* @note This function does not take exceptions into account. For example it will return "A Nice Day On Twenty-third Street" not "A Nice Day on Twenty-Third Street".
* @param str the string to operate on
*
* @returns the titlecase string
*/
std::string string::title(const std::string& str) {
	std::string s = "";
	s.reserve(str.length());

	char previous_c = ' ';
	for (auto c : str) {
		if (previous_c == ' ') {
			s += ::toupper(c);
		} else {
			s += ::tolower(c);
		}

		previous_c = c;
	}

	return s;
}
/**
* Filter the characters in the string so that only alphabetical characters are returned.
* @param str the string to operate on
*
* @returns the filtered string
*/
std::string string::letters(const std::string& str) {
	return std::regex_replace(str, std::regex("[^[:alpha:]]"), std::string(""));
}
/**
* Filter the characters in the string so that only digits are returned.
* @param str the string to operate on
*
* @returns the filtered string
*/
std::string string::digits(const std::string& str) {
	return std::regex_replace(str, std::regex("[^[:digit:]]"), std::string(""));
}
/**
* Filter the characters in the string so that only alphanumeric characters are returned.
* @param str the string to operate on
*
* @returns the filtered string
*/
std::string string::lettersdigits(const std::string& str) {
	return std::regex_replace(str, std::regex("[^[:alnum:]]"), std::string(""));
}

/**
* Convert the given string to a boolean value.
* @note Currently everything except "0" and "false" will evaluate to true.
* @param str the string to convert
*
* @returns the corresponding boolean value
*/
bool string::tobool(const std::string& str) {
	std::string lstr = lower(trim(str)); // Trim the string and convert it to lowercase
	std::vector<std::string> false_values = {"0", "false"}; // Declare the values which shall evaluate to false
	for (auto& s : false_values) { // Iterate over the false values and compare them to the string
		if (lstr == s) {
			return false; // Return false if the string is one of the false values
		}
	}
	return true; // Return true if the string is not any of the false values
}
/**
* Convert the given boolean value to a string.
* @param b the bool to convert
*
* @returns the corresponding string
*/
std::string string::frombool(bool b) {
	if (b) {
		return "true"; // Return "true" if true
	}
	return "false"; // Otherwise, return false
}

/**
* Replace all occurences of the given search string with the given replacement.
* @param str the string to operate on
* @param search the string to search and replace
* @param replacment the string to replace the search
*
* @returns the string with all occurences replaced
*/
std::string string::replace(const std::string& str, const std::string& search, const std::string& replacement) {
	std::string s (str); // Create a new string so that the given string is not modified
	size_t start_pos = 0; // Start at the beginning of the string
	while ((start_pos = s.find(search, start_pos)) != std::string::npos) { // Continue to search through the string for the given substring
		s.replace(start_pos, search.length(), replacement); // Replace the substring
		start_pos += replacement.length(); // Start from the end of the replacement
	}
	return s; // Return the modified string
}
/**
* Replace all occurences of the quotes and back slashes with escaped versions.
* @param str the string to operate on
*
* @returns the escaped string
*/
std::string string::escape(const std::string& str) {
	std::string s (str);
	s = replace(s, "\\", "\\\\"); // Escape back slashes
	s = replace(s, "\"", "\\\""); // Escape quotes
	return s;
}
/**
* Replace all occurences of escaped quotes and back slashes with unescaped versions.
* @param str the string to operate on
*
* @returns the original unescaped string
*/
std::string string::unescape(const std::string& str) {
	std::string s (str);
	s = replace(s, "\\\\", "\\"); // Replace escaped back slashes
	s = replace(s, "\\\"", "\""); // Replace escaped quotes
	return s;
}
/**
* Repeat a given string the given number of times.
* @param amount the number of times to repeat the string
* @param str the string to repeat
*
* @returns the repeated string
*/
std::string string::repeat(size_t amount, const std::string& str) {
	if (str.length() == 1) { // If the string is only a single character long
		return std::string(amount, str[0]); // Return the string initializer which implements single-character repetition
	}

	std::string s;
	for (size_t i=0; i<amount; i++) { // Continually append the string as needed
		s += str;
	}
	return s;
}

/**
* Tabulate a table of strings such that the columns have at least one space between them.
* @note All sub-vectors should have the same number of elements.
* @param table the table to tabulate
*
* @returns the tabulated string
*/
std::string string::tabulate(const std::vector<std::vector<std::string>>& table) {
	std::vector<size_t> column_width; // Create a vector which measures the width of each existing column
	for (size_t i=0; i<table[0].size(); i++) { // Iterate over the columns of the first row
		column_width.push_back(0); // Push an initial width for every column
	}

	for (auto& r : table) { // Iterate over the table rows
		for (size_t i=0; i<r.size(); i++) { // Iterate over the table columns
			if (r[i].length() > column_width[i]) { // If the length of this value is greater than the column width
				column_width[i] = r[i].length(); // Expand the column width to fit it
			}
		}
	}

	std::string str;
	for (auto& r : table) { // Iterate over the table rows
		for (size_t i=0; i<r.size(); i++) { // Iterate over the table columns
			str += r[i] + repeat(column_width[i]-r[i].length()+1, " "); // Append the columns to the tabulation
		}
		str += "\n"; // Separate each row with a new line
	}

	return str;
}

/**
* @param str the string to evaluate
*
* @returns whether the string is a floating point number or not
*/
bool string::is_floating(const std::string& str) {
	std::string ns = str;
	if (ns.empty()) {
		return false;
	}

	// Remove the number's sign if it has one
	if ((ns[0] == '-')||(ns[0] == '+')) {
		ns = ns.substr(1);
	}

	// Remove any scientific notation
	static const std::vector<std::string> exps = {"e", "E"};
	for (auto& e : exps) {
		if (ns.find(e) != std::string::npos) {
			size_t exponent = ns.find(e);
			if (!is_integer(ns.substr(exponent+1))) { // If the exponent is not an integer, it is not valid scientific notation
				return false;
			}

			ns.erase(exponent);
			break; // After removing the notation, continue below
		}
	}

	// Remove a single decimal point
	if (ns.find(".") != std::string::npos) {
		ns.erase(ns.find("."));
	}

	// Remove all digits
	static const std::vector<std::string> digits = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	for (auto& d : digits) {
		ns = replace(ns, d, "");
	}

	if (!ns.empty()) {
		return false;
	}
	return true;
}
/**
* @param str the string to evaluate
*
* @returns whether the string is an integer
*/
bool string::is_integer(const std::string& str) {
	std::string ns = str;
	if (ns.empty()) {
		return false;
	}

	// Remove the number's sign if it has one
	if ((ns[0] == '-')||(ns[0] == '+')) {
		ns = ns.substr(1);
	}

	// Remove all digits
	static const std::vector<std::string> digits = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
	for (auto& d : digits) {
		ns = replace(ns, d, "");
	}

	if (!ns.empty()) {
		return false;
	}
	return true;
}

}

#endif // BEE_UTIL_STRING
