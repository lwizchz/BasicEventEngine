/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_CURL_H
#define BEE_UTIL_CURL_H 1

#include <string>
#include <functional>

#include <curl/curl.h>

namespace util { namespace curl {
	int init();
	int download(const std::string&, const std::string&, std::function<int (curl_off_t, curl_off_t)>);
	int download(const std::string&, const std::string&);
	int upload(const std::string&, const std::string&, std::function<int (curl_off_t, curl_off_t)>);
	int upload(const std::string&, const std::string&);
}}

#endif // BEE_UTIL_CURL_H
