/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_PLATFORM
#define TESTS_UTIL_PLATFORM 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/platform.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("platform/general") {
	REQUIRE(bee_get_platform() >= 0);
	REQUIRE(bee_get_platform() < 3);
	REQUIRE(!bee_get_path().empty());

	REQUIRE(bee_itos(0) == "0");
	REQUIRE(bee_itos(2) == "2");
	REQUIRE(bee_itos(100) == "100");

	REQUIRE(bee_stoi("0") == 0);
	REQUIRE(bee_stoi("2") == 2);
	REQUIRE(bee_stoi("100") == 100);

	REQUIRE(bee_stoi(bee_itos(0)) == 0);
	REQUIRE(bee_stoi(bee_itos(2)) == 2);
	REQUIRE(bee_stoi(bee_itos(100)) == 100);

	REQUIRE(bee_itos(bee_stoi("0")) == "0");
	REQUIRE(bee_itos(bee_stoi("2")) == "2");
	REQUIRE(bee_itos(bee_stoi("100")) == "100");
}
TEST_CASE("platform/files") {
	std::string tmpdir = bee_mkdtemp("/tmp/bee-XXXXXX");
	REQUIRE(tmpdir != "");
	tmpdir += "/";
	REQUIRE(bee_dir_exists(tmpdir) == true);
	REQUIRE(bee_mkdir(tmpdir+"log/", 0755) == 0);
	REQUIRE(bee_remove(tmpdir+"log/") == 0);
	REQUIRE(bee_remove(tmpdir) == 0);
}
TEST_CASE("platform/network") {
	//REQUIRE(bee_inet_ntop() == "127.0.0.1");
}
TEST_CASE("platform/commandline") {
	//REQUIRE(bee_has_commandline_input() == false);
}

TEST_SUITE_END();

#endif // TESTS_UTIL_PLATFORM
