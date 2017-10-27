/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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
	int c1 = choose<int>({1, 2, 3, 4});
	REQUIRE(((c1 == 1)||(c1 == 2)||(c1 == 3)||(c1 == 4)));
	double c2 = choose<double>({1.0, 2.0, 3.0, 4.0});
	REQUIRE(((c2 == 1.0)||(c2 == 2.0)||(c2 == 3.0)||(c2 == 4.0)));
	std::string c3 = choose<std::string>({"a", "b", "c", "d"});
	REQUIRE(((c3 == "a")||(c3 == "b")||(c3 == "c")||(c3 == "d")));

	int mean1 = mean<int>({1, 2, 3, 4});
	REQUIRE(mean1 == 2);
	double mean2 = mean<double>({1.0, 2.0, 3.0, 4.0});
	REQUIRE(mean2 == 2.5);

	int median1 = median<int>({1, 2, 3, 4});
	REQUIRE(median1 == 2);
	int median2 = median<int>({1, 2, 3, 4, 5});
	REQUIRE(median2 == 3);
	double median3 = median<double>({1.0, 2.0, 3.0, 4.0});
	REQUIRE(median3 == 2.5);
	double median4 = median<double>({1.0, 2.0, 3.0, 4.0, 5.0});
	REQUIRE(median4 == 3.0);
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

	REQUIRE(map_serialize(m, false) == "{a:1,b:2,c:3,d:4}");
	REQUIRE(map_deserialize("{a:1,b:2,c:3,d:4}", &n1) == 0);
	REQUIRE(std::equal(m.begin(), m.end(), n1.begin()));
	REQUIRE(map_deserialize(map_serialize(m, true), &n2) == 0);
	REQUIRE(std::equal(m.begin(), m.end(), n2.begin()));
}

TEST_SUITE_END();

#endif // TESTS_UTIL_TEMPLATE
