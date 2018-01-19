/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_FILES
#define TESTS_UTIL_FILES 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/files.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("files") {
	std::string tmpdir = util::directory_get_temp();
	REQUIRE(tmpdir != "");
	REQUIRE(util::directory_exists(tmpdir) == true);
	REQUIRE(util::directory_create(tmpdir+"log/") == 0);
	REQUIRE(util::file_put_contents(tmpdir+"test.txt", tmpdir) == tmpdir.size());
	REQUIRE(util::file_get_contents(tmpdir+"test.txt") == tmpdir);
	REQUIRE(util::file_rename(tmpdir+"test.txt", tmpdir+"log/test.txt") == 0);
	REQUIRE(util::file_copy(tmpdir+"log/test.txt", tmpdir+"test.txt") == 0);
	REQUIRE(util::file_exists(tmpdir+"log/test.txt") == true);
	REQUIRE(util::file_delete(tmpdir+"log/test.txt") == 0);
	REQUIRE(util::file_delete(tmpdir+"log/") == 0);
	REQUIRE(util::file_delete(tmpdir+"test.txt") == 0);
	REQUIRE(util::file_delete(tmpdir) == 0);

	REQUIRE(util::file_basename(tmpdir+"log/test.txt") == "test.txt");
	REQUIRE(util::file_dirname(tmpdir+"log/test.txt") == tmpdir+"log");
	REQUIRE(util::file_plainname(tmpdir+"log/test.txt") == tmpdir+"log/test");
	REQUIRE(util::file_extname(tmpdir+"log/test.txt") == ".txt");

	REQUIRE(util::file_basename("test.txt") == "test.txt");
	REQUIRE(util::file_basename("/tmp/log/") == "");
	REQUIRE(util::file_dirname("test.txt") == ".");
	REQUIRE(util::file_plainname("/tmp/test") == "/tmp/test");
	REQUIRE(util::file_extname("test") == "");
}

TEST_SUITE_END();

#endif // TESTS_UTIL_FILES
