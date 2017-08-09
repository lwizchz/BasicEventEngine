/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_TEMPLATE_NETWORK_H
#define BEE_UTIL_TEMPLATE_NETWORK_H 1

// Network template-requiring functions

#include <string> // Include the required library headers
#include <algorithm>
#include <sstream>
#include <map>

#include "string.hpp"

/*
* network_map_encode() - Return an array of Uint8's which represent a map
* @m: the map to encode
*/
template <typename A, typename B>
std::pair<size_t,Uint8*> network_map_encode(std::map<A,B> m) {
	return orda(map_serialize(m, false)); // Return the string after converting it into an array of Uint8's
}
/*
* network_map_decode() - Decode the given array of Uint8's into the given map
* @size: the size of the data array
* @data: the array to decode
* @mv: the map to store the values in
*/
template <typename A, typename B>
int network_map_decode(size_t size, Uint8* data, std::map<A,B>* mv) {
	std::string datastr = chra(size, data); // Convert the array to a string
	map_deserialize(datastr, mv);
	return 0; // Return 0 on success
}
/*
* network_map_decode() - Decode the given array of Uint8's into the given map
* @data: the array to decode
* @mv: the map to store the values in
*/
template <typename A, typename B>
int network_map_decode(std::pair<size_t,Uint8*> data, std::map<A,B>* mv) {
	return network_map_decode(data.first, data.second, mv);
}

#endif // BEE_UTIL_TEMPLATE_NETWORK_H
