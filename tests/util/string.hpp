/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_STRING
#define TESTS_UTIL_STRING 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/string.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("string/charcode") {
	REQUIRE(util::chr(65) == "A");
	Uint8 ca1[] = {65, 66, 67};
	REQUIRE(util::chra(3, ca1) == std::string("ABC"));
	std::vector<Uint8> ca2 = util::orda("ABC");
	REQUIRE(util::chra(ca2) == util::chra(ca2.size(), ca1));
}
TEST_CASE("string/vector") {
	std::vector<std::string> v1 = {"ab", "1.0", "\"c d\"", "test"};
	REQUIRE(util::splitv("ab 1.0 \"c d\" test", ' ', true) == v1);

	std::vector<std::string> v2 = {"ab", "1.0", "test"};
	REQUIRE(util::splitv("ab 1.0 test", ' ', true) == v2);

	std::vector<std::string> v3 = {"a", "b", "c"};
	REQUIRE(util::splitv("a,b,c", ',', true) == v3);
	REQUIRE(util::joinv(v3, ',') == "a,b,c");
	REQUIRE(util::splitv(util::joinv(v3, ','), ',', true) == v3);

	std::vector<std::string> v4 = {"\"a", "b\"", "c"};
	std::vector<std::string> vr4 = {"\"a,b\"", "c"};
	REQUIRE(util::splitv(util::joinv(v4, ','), ',', true) == vr4);

	std::vector<std::string> v5 = {"ab \"a;b\" \"b\"", " cd ef", " \"g;\"", " h"};
	REQUIRE(util::splitv("ab \"a;b\" \"b\"; cd ef; \"g;\"; h", ';', true) == v5);
}
TEST_CASE("string/manipulation") {
	REQUIRE(util::ltrim("  ABC  ") == "ABC  ");
	REQUIRE(util::rtrim("  ABC  ") == "  ABC");
	REQUIRE(util::trim("  ABC  ") == "ABC");

	/*REQUIRE(util::clipboard_set_text("test") == 0); // These functions require SDL to be initialized
	REQUIRE(util::clipboard_get_text() == "test");
	REQUIRE(util::clipboard_has_text() == true);*/

	REQUIRE(util::string::lower("ABC") == "abc");
	REQUIRE(util::string::upper("abc") == "ABC");
	REQUIRE(util::string::title("abc") == "Abc");
	REQUIRE(util::string::title("abc def") == "Abc Def");
	REQUIRE(util::string::letters("ABC123,./") == "ABC");
	REQUIRE(util::string::digits("ABC123,./") == "123");
	REQUIRE(util::string::lettersdigits("ABC123,./") == "ABC123");

	REQUIRE(util::string::tobool("true") == true);
	REQUIRE(util::string::tobool("false") == false);
	REQUIRE(util::string::tobool("0") == false);
	REQUIRE(util::string::tobool("1") == true);
	REQUIRE(util::string::tobool("True") == true);
	REQUIRE(util::string::tobool("False") == false);
	REQUIRE(util::string::tobool("random text") == true);
	REQUIRE(util::string::tobool("20") == true);

	REQUIRE(util::string::frombool(true) == "true");
	REQUIRE(util::string::frombool(false) == "false");

	REQUIRE(util::string::replace("a,b,c", ",", ":") == "a:b:c");
	REQUIRE(util::string::replace("a:test:b:test:c", ":test:", ",") == "a,b,c");

	REQUIRE(util::string::escape("\"Test\"/\\test") == "\\\"Test\\\"/\\\\test");
	REQUIRE(util::string::unescape("\\\"Test\\\"/\\\\test") == "\"Test\"/\\test");
	REQUIRE(util::string::unescape(util::string::escape("\"Test\"/\\test")) == "\"Test\"/\\test");
}

TEST_SUITE_END();

#endif // TESTS_UTIL_STRING
