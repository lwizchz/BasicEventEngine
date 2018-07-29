/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_DEBUG
#define TESTS_UTIL_DEBUG 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/debug.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("debug") {
	std::string abc = "a\nb\nc";
	REQUIRE(util::debug_indent(abc, 1) == "\ta\n\tb\n\tc\n");
}

TEST_SUITE_END();

#endif // TESTS_UTIL_DEBUG
