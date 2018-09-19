/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_DATA_SERIALDATA
#define TESTS_DATA_SERIALDATA 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/data/serialdata.hpp"

TEST_SUITE_BEGIN("data");

TEST_CASE("serialdata1") {
	bee::SerialData sd1;
	sd1.store(static_cast<unsigned char>('H')); // These must be cast otherwise they'll be upcast to ints
	sd1.store(static_cast<unsigned char>('i'));
	sd1.store(24l);
	sd1.store(3.1415);
	sd1.store(std::string("Hello!"));

	bee::SerialData sd2 (sd1.get());
	unsigned char a, b;
	long c;
	double d;
	std::string e;
	sd2.get(a);
	sd2.get(b);
	sd2.get(c);
	sd2.get(d);
	sd2.get(e);

	REQUIRE(sd1.get() == sd2.get());
	REQUIRE(a == 'H');
	REQUIRE(b == 'i');
	REQUIRE(c == 24);
	REQUIRE(d == 3.1415);
	REQUIRE(e == "Hello!");
}

TEST_CASE("serialdata2") {
	unsigned char a1 = 'H', a2;
	unsigned char b1 = 'i', b2;
	long c1 = 24, c2;
	double d1 = 3.1415, d2;
	std::string e1 = "Hello!", e2;

	// Writing
	bee::SerialData sd1;
	sd1.store(a1);
	sd1.store(b1);
	sd1.store(c1);
	sd1.store(d1);
	sd1.store(e1);

	// Reading
	bee::SerialData sd2 (sd1.get());
	sd2.get(a2);
	sd2.get(b2);
	sd2.get(c2);
	sd2.get(d2);
	sd2.get(e2);

	REQUIRE(sd1.get() == sd2.get());
	REQUIRE(a1 == a2);
	REQUIRE(b1 == b2);
	REQUIRE(c1 == c2);
	REQUIRE(d1 == d2);
	REQUIRE(e1 == e2);
}

TEST_CASE("serialdata3") {
	unsigned char a1 = 'H', a2;
	unsigned char b1 = 'i', b2;
	long c1 = 24, c2;
	double d1 = 3.1415, d2;
	std::string e1 = "Hello!", e2;

	// Writing
	bee::SerialData sd1;
	sd1.store_char(a1);
	sd1.store_char(b1);
	sd1.store_long(c1);
	sd1.store_double(d1);
	sd1.store_string(e1);

	// Reading
	bee::SerialData sd2 (sd1.get());
	sd2.store_char(a2);
	sd2.store_char(b2);
	sd2.store_long(c2);
	sd2.store_double(d2);
	sd2.store_string(e2);

	REQUIRE(sd1.get() == sd2.get());
	REQUIRE(a1 == a2);
	REQUIRE(b1 == b2);
	REQUIRE(c1 == c2);
	REQUIRE(d1 == d2);
	REQUIRE(e1 == e2);
}

TEST_CASE("serialdata4") {
	std::vector<std::string> a1 = {
		"config_value1 = 1",
		"config_value2 = 2",
		"config_value3 = 3",
		"config_value4 = 4",
	};
	std::vector<std::string> a2, a3;

	std::map<std::string,int> b1 {
		{"config_value1", 1},
		{"config_value2", 2},
		{"config_value3", 3},
		{"config_value4", 4}
	};
	std::map<std::string,int> b2, b3;

	// Writing
	bee::SerialData sd1;
	sd1.store_vector(a1);
	sd1.store_map(b1);

	// Reading
	bee::SerialData sd2 (sd1.get());
	sd2.store_vector(a2);
	sd2.store_map(b2);

	sd2.rewind();
	sd2.store_vector(a3);
	sd2.store_map(b3);

	REQUIRE(sd1.get() == sd2.get());
	REQUIRE(a1 == a2);
	REQUIRE(b1 == b2);
	REQUIRE(a1 == a3);
	REQUIRE(b1 == b3);
}

TEST_SUITE_END();

#endif // TESTS_DATA_SERIALDATA
