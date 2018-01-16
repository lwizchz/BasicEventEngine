/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_DATA_VARIANT
#define TESTS_DATA_VARIANT 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/data/variant.hpp"

TEST_SUITE_BEGIN("data");

TEST_CASE("variant1") {
	bee::Variant v1 (bee::E_DATA_TYPE::CHAR);
	v1.c = 'A';

	bee::Variant v2 (v1);

	char c = 'A';

	REQUIRE(v1 == v2);
	REQUIRE(v1.c == c);
}
TEST_CASE("variant2") {
	bee::Variant v1 (bee::E_DATA_TYPE::MAP);
	v1.m.emplace(bee::Variant("a"), bee::Variant(2.0));

	bee::Variant v2 ("{\"a\": 2.0}", true);

	std::map<bee::Variant,bee::Variant> m;
	m.emplace(bee::Variant("a"), bee::Variant(2.0));

	REQUIRE(v1 == v2);
	REQUIRE(v1.m == m);
	REQUIRE(v1.m.at("a") == m.at("a"));
	REQUIRE(v1.m.find("b") == v1.m.end());
	REQUIRE(v1.m.size() == 1);
}

TEST_SUITE_END();

#endif // TESTS_DATA_VARIANT
