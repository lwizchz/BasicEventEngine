/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_TEMPLATE
#define TESTS_UTIL_TEMPLATE 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/template.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("template/real") {
	int c1 = util::choose<int>({1, 2, 3, 4});
	REQUIRE(((c1 == 1)||(c1 == 2)||(c1 == 3)||(c1 == 4)));
	double c2 = util::choose<double>({1.0, 2.0, 3.0, 4.0});
	REQUIRE(((c2 == 1.0)||(c2 == 2.0)||(c2 == 3.0)||(c2 == 4.0)));
	std::string c3 = util::choose<std::string>({"a", "b", "c", "d"});
	REQUIRE(((c3 == "a")||(c3 == "b")||(c3 == "c")||(c3 == "d")));

	REQUIRE(util::mean<int>({1, 2, 3, 4}) == 2);
	REQUIRE(util::mean<double>({1.0, 2.0, 3.0, 4.0}) == 2.5);

	REQUIRE(util::median<int>({1, 2, 3, 4}) == 2);
	REQUIRE(util::median<int>({1, 2, 3, 4, 5}) == 3);
	REQUIRE(util::median<double>({1.0, 2.0, 3.0, 4.0}) == 2.5);
	REQUIRE(util::median<double>({1.0, 2.0, 3.0, 4.0, 5.0}) == 3.0);
}
TEST_CASE("template/string") {
	std::map<std::string,int> m = {
		{ "a", 1 },
		{ "b", 2 },
		{ "c", 3 },
		{ "d", 4 }
	};
	std::map<std::string,int> n1;
	std::map<std::string,int> n2;

	REQUIRE(util::map_serialize(m, false) == "{a:1,b:2,c:3,d:4}");
	REQUIRE(util::map_deserialize("{a:1,b:2,c:3,d:4}", &n1) == 0);
	REQUIRE(std::equal(m.begin(), m.end(), n1.begin()));
	REQUIRE(util::map_deserialize(util::map_serialize(m, true), &n2) == 0);
	REQUIRE(std::equal(m.begin(), m.end(), n2.begin()));
}

TEST_SUITE_END();

#endif // TESTS_UTIL_TEMPLATE
