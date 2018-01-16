/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_STRING
#define TESTS_UTIL_STRING 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/string.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("string/charcode") {
	REQUIRE(chr(65) == "A");
	REQUIRE(ord('A') == 65);
	REQUIRE(ord("ABC") == 65);
	Uint8 ca1[] = {65, 66, 67};
	REQUIRE(chra(3, ca1) == std::string("ABC"));
	std::vector<Uint8> ca2 = orda("ABC");
	REQUIRE(chra(ca2) == chra(ca2.size(), ca1));
}
TEST_CASE("string/alteration") {
	REQUIRE(string_lower("ABC") == "abc");
	REQUIRE(string_upper("abc") == "ABC");
	REQUIRE(string_title("abc") == "Abc");
	REQUIRE(string_title("abc def") == "Abc Def");
	REQUIRE(string_letters("ABC123,./") == "ABC");
	REQUIRE(string_digits("ABC123,./") == "123");
	REQUIRE(string_lettersdigits("ABC123,./") == "ABC123");

	std::map<int,std::string> m1 = {{0, "ab"}, {1, "1.0"}, {2, "\"c d\""}, {3, "test"}};
	REQUIRE(split("ab 1.0 \"c d\" test", ' ', true) == m1);

	std::map<int,std::string> m2 = {{0, "ab"}, {1, "1.0"}, {2, "test"}};
	REQUIRE(split("ab 1.0 test", ' ', true) == m2);

	std::map<int,std::string> m3 = {{0, "a"}, {1, "b"}, {2, "c"}};
	REQUIRE(split("a,b,c", ',') == m3);
	REQUIRE(handle_newlines("a\nb\nc") == m3);
	REQUIRE(join(m3, ',') == "a,b,c");
	REQUIRE(split(join(m3, ','), ',') == m3);

	std::map<int,std::string> m4 = {{0, "\"a"}, {1, "b\""}, {2, "c"}};
	std::map<int,std::string> mr4 = {{0, "\"a,b\""}, {1, "c"}};
	REQUIRE(split(join(m4, ','), ',', true) == mr4);

	std::map<int,std::string> m5 = {{0, "ab \"a;b\" \"b\""}, {1, " cd ef"}, {2, " \"g;\""}, {3, " h"}};
	REQUIRE(split("ab \"a;b\" \"b\"; cd ef; \"g;\"; h", ';', true) == m5);

	REQUIRE(ltrim("  ABC  ") == "ABC  ");
	REQUIRE(rtrim("  ABC  ") == "  ABC");
	REQUIRE(trim("  ABC  ") == "ABC");
}
TEST_CASE("string/misc") {
	REQUIRE(stringtobool("true") == true);
	REQUIRE(stringtobool("false") == false);
	REQUIRE(stringtobool("0") == false);
	REQUIRE(stringtobool("1") == true);
	REQUIRE(stringtobool("True") == true);
	REQUIRE(stringtobool("False") == false);
	REQUIRE(stringtobool("random text") == true);
	REQUIRE(stringtobool("20") == true);

	REQUIRE(booltostring(true) == "true");
	REQUIRE(booltostring(false) == "false");

	REQUIRE(string_replace("a,b,c", ",", ":") == "a:b:c");
	REQUIRE(string_replace("a:test:b:test:c", ":test:", ",") == "a,b,c");

	REQUIRE(string_escape("\"Test\"/\\test") == "\\\"Test\\\"/\\\\test");
	REQUIRE(string_unescape("\\\"Test\\\"/\\\\test") == "\"Test\"/\\test");
	REQUIRE(string_unescape(string_escape("\"Test\"/\\test")) == "\"Test\"/\\test");

	/*REQUIRE(clipboard_set_text("test") == 0); // These functions require SDL to be initialized
	REQUIRE(clipboard_get_text() == "test");
	REQUIRE(clipboard_has_text() == true);*/
}

TEST_SUITE_END();

#endif // TESTS_UTIL_STRING
