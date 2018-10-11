/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_ARCHIVE
#define TESTS_UTIL_ARCHIVE 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/archive.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("archive") {
	std::string tmpdir = util::directory_get_temp();
	REQUIRE(util::directory_exists(tmpdir) == true);

	REQUIRE(util::directory_create(tmpdir+"archive.old/") == 0);
	REQUIRE(util::file_put_contents(tmpdir+"archive.old/a.txt", "abc\n") == 4);
	REQUIRE(util::archive::tar_create(tmpdir+"archive.old") == 0);
	REQUIRE(util::file_exists(tmpdir+"archive.old.tar") == true);
	REQUIRE(util::archive::xz_compress(tmpdir+"archive.old.tar") == 0);
	REQUIRE(util::file_exists(tmpdir+"archive.old.tar.xz") == true);

	REQUIRE(util::archive::xz_decompress(tmpdir+"archive.old.tar.xz", tmpdir+"archive.new.tar") == 0);
	REQUIRE(util::file_exists(tmpdir+"archive.new.tar") == true);
	REQUIRE(util::archive::tar_extract(tmpdir+"archive.new.tar") == 0);
	REQUIRE(util::directory_exists(tmpdir+"archive.new/") == true);
	REQUIRE(util::file_exists(tmpdir+"archive.new/a.txt") == true);
	REQUIRE(util::file_get_contents(tmpdir+"archive.new/a.txt") == "abc\n");
}

TEST_SUITE_END();

#endif // TESTS_UTIL_ARCHIVE
