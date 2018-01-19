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
	REQUIRE(util::platform::get_platform() >= 0);
	REQUIRE(util::platform::get_platform() < 3);
	REQUIRE(!util::platform::get_path().empty());
}
TEST_CASE("platform/files") {
	std::string tmpdir = util::platform::mkdtemp("/tmp/bee-XXXXXX");
	REQUIRE(tmpdir != "");
	tmpdir += "/";
	REQUIRE(util::platform::dir_exists(tmpdir) == true);
	REQUIRE(util::platform::mkdir(tmpdir+"log/", 0755) == 0);
	REQUIRE(util::platform::remove(tmpdir+"log/") == 0);
	REQUIRE(util::platform::remove(tmpdir) == 0);
}
TEST_CASE("platform/network") {
	//REQUIRE(util::platform::inet_ntop() == "127.0.0.1");
}
TEST_CASE("platform/commandline") {
	//REQUIRE(util::platform::has_commandline_input() == false);
}

TEST_SUITE_END();

#endif // TESTS_UTIL_PLATFORM
