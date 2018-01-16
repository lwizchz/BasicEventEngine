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
	std::string tmpdir = directory_get_temp();
	REQUIRE(tmpdir != "");
	REQUIRE(directory_exists(tmpdir) == true);
	REQUIRE(directory_create(tmpdir+"log/") == 0);
	REQUIRE(file_put_contents(tmpdir+"test.txt", tmpdir) == tmpdir.size());
	REQUIRE(file_get_contents(tmpdir+"test.txt") == tmpdir);
	REQUIRE(file_rename(tmpdir+"test.txt", tmpdir+"log/test.txt") == 0);
	REQUIRE(file_copy(tmpdir+"log/test.txt", tmpdir+"test.txt") == 0);
	REQUIRE(file_exists(tmpdir+"log/test.txt") == true);
	REQUIRE(file_delete(tmpdir+"log/test.txt") == 0);
	REQUIRE(file_delete(tmpdir+"log/") == 0);
	REQUIRE(file_delete(tmpdir+"test.txt") == 0);
	REQUIRE(file_delete(tmpdir) == 0);

	REQUIRE(file_basename(tmpdir+"log/test.txt") == "test.txt");
	REQUIRE(file_dirname(tmpdir+"log/test.txt") == tmpdir+"log/");
	REQUIRE(file_plainname(tmpdir+"log/test.txt") == tmpdir+"log/test");
	REQUIRE(file_extname(tmpdir+"log/test.txt") == ".txt");
}

TEST_SUITE_END();

#endif // TESTS_UTIL_FILES
