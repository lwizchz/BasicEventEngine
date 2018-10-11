/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_CURL
#define TESTS_UTIL_CURL 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/curl.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("curl") {
	std::string tmpdir = util::directory_get_temp();
	REQUIRE(util::directory_exists(tmpdir) == true);
	REQUIRE(util::directory_create(tmpdir+"maps/") == 0);

	REQUIRE(util::curl::download("https://lukemontalvo.us/BasicEventEngine/maps/lv_test.tar.xz", tmpdir+"maps/new_map.tar.xz", [] (curl_off_t total, curl_off_t now) -> int {
		double percent = (double)now/total*100;
		INFO("\rdownloading " << percent << "%...     ");
		return 0;
	}) == 0);
	INFO("\n");
	REQUIRE(util::curl::upload("https://lukemontalvo.us/BasicEventEngine/upload.php/new_map.tar.xz?test", tmpdir+"maps/new_map.tar.xz", [] (curl_off_t total, curl_off_t now) -> int {
		double percent = (double)now/total*100;
		INFO("\ruploading " << percent << "%...     ");
		return 0;
	}) == 0);
	INFO("\n");
}

TEST_SUITE_END();

#endif // TESTS_UTIL_CURL
