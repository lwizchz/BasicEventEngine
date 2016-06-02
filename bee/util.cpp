/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL
#define _BEE_UTIL 1

#include "util.hpp" // Include the function declarations

#include "util/platform.hpp" // Include the platform compatibility functions
#include "util/real.hpp" // Include the real number functions
#include "util/string.hpp" // Include the string handling functions
#include "util/dates.hpp" // Include the date and time funcitons
#include "util/collision.hpp" // Include the collision checking functions
#include "util/sound.hpp" // Include the sound effect functions
#include "util/messagebox.hpp" // Include the message box functions
#include "util/files.hpp" // Include the file handling functions
#include "util/network.hpp" // Include the networking functions

#ifndef NDEBUG

#define DOCTEST_CONFIG_IMPLEMENT
#include "../lib/doctest.h"

// Real number function assertions
TEST_CASE("real/random") {
	REQUIRE(random(20) < (unsigned int)20);
	REQUIRE(is_between((int)random_range(20, 40), 20, 40));
	REQUIRE(random_set_seed(5) == (unsigned int)5);
	REQUIRE(random_reset_seed() != (unsigned int)DEFAULT_RANDOM_SEED);
	REQUIRE(randomize() != (unsigned int)DEFAULT_RANDOM_SEED);
}
TEST_CASE("real/math") {
	REQUIRE(sign(5) == 1);
	REQUIRE(sign(0) == 0);
	REQUIRE(sign(-5) == -1);
	REQUIRE(sign(5.0) == 1);
	REQUIRE(sign(0.0) == 0);
	REQUIRE(sign(-5.0) == -1);
	REQUIRE(sqr(5) == 25);
	REQUIRE(sqr(5.0) == 25.0);
	REQUIRE(logn(5.0, 1.0) == 0.0);
	REQUIRE(logn(5.0, 5.0) == 1.0);
	REQUIRE(logn(5.0, 10.0) == doctest::Approx(1.431).epsilon(0.001));
	REQUIRE(degtorad(90.0) == doctest::Approx(PI/2.0));
	REQUIRE(degtorad(360.0) == doctest::Approx(2.0*PI));
	REQUIRE(radtodeg(PI) == doctest::Approx(180.0));
	REQUIRE(radtodeg(PI/3.0) == doctest::Approx(60.0));
	REQUIRE(opposite_angle(0.0) == 180.0);
	REQUIRE(opposite_angle(60.0) == 240.0);
	REQUIRE(opposite_angle(270.0) == 90.0);
	REQUIRE(opposite_angle(360.0) == 180.0);
	REQUIRE(opposite_angle(-90.0) == 90.0);
}
TEST_CASE("real/movement") {
	REQUIRE(direction_of(1.0, 2.0, 3.0, 4.0) == doctest::Approx(opposite_angle(direction_of(3.0, 4.0, 1.0, 2.0))));
	REQUIRE(direction_of(0.0, 0.0, 1.0, 0.0) == doctest::Approx(0.0));
	REQUIRE(direction_of(0.0, 0.0, 0.0, 1.0) == doctest::Approx(90.0));
	REQUIRE(direction_of(0.0, 0.0, -1.0, 0.0) == doctest::Approx(180.0));
	REQUIRE(direction_of(0.0, 0.0, 0.0, -1.0) == doctest::Approx(270.0));
	REQUIRE(distance(0.0, 0.0, 1.0, 0.0) == 1.0);
	REQUIRE(distance(0.0, 0.0, 3.0, 4.0) == 5.0);
	REQUIRE(distance(0.0, 0.0, 0.0, 0.0) == 0.0);
}
TEST_CASE("real/dot_product") {
	REQUIRE(dot_product(1, 2, 3, 4) == 11);
	REQUIRE(dot_product(1.0, 2.0, 3.0, 4.0) == 11.0);
	REQUIRE(dot_product(std::make_pair(1, 2), std::make_pair(3, 4)) == 11);
}
TEST_CASE("real/bounds") {
	REQUIRE(is_between(5, 3, 6) == true);
	REQUIRE(is_between(6, 3, 6) == true);
	REQUIRE(is_between(7, 3, 6) == false);
	REQUIRE(is_between(5, 6, 3) == true);
	REQUIRE(is_between(5.0, 3.0, 6.0) == true);
	REQUIRE(is_angle_between(90, 0, 180) == true);
	REQUIRE(is_angle_between(180, 0, 180) == true);
	REQUIRE(is_angle_between(270, 0, 180) == false);
	REQUIRE(is_angle_between(90.0, 0.0, 180.0) == true);
	REQUIRE(is_angle_between(0, 270, 90) == true);
	REQUIRE(is_angle_between(360, 270, 90) == true);
	REQUIRE(fit_bounds(5, 3, 6) == 5);
	REQUIRE(fit_bounds(6, 3, 6) == 6);
	REQUIRE(fit_bounds(7, 3, 6) == 6);
	REQUIRE(fit_bounds(5, 6, 3) == 5);
	REQUIRE(fit_bounds(5.0, 3.0, 6.0) == 5.0);
}

// String handling function assertions
TEST_CASE("string/charcode") {
	REQUIRE(chr(65) == "A");
	REQUIRE(ord('A') == 65);
	REQUIRE(ord("ABC") == 65);
	Uint8 ca[] = {3, 65, 66, 67};
	REQUIRE(chra(ca) == std::string("ABC"));
	REQUIRE(chra(orda("ABC")) == chra(ca));
}
TEST_CASE("string/alteration") {
	REQUIRE(string_lower("ABC") == "abc");
	REQUIRE(string_upper("abc") == "ABC");
	REQUIRE(string_letters("ABC123,./") == "ABC");
	REQUIRE(string_digits("ABC123,./") == "123");
	REQUIRE(string_lettersdigits("ABC123,./") == "ABC123");

	std::map<int,std::string> m = {{0, "a"}, {1, "b"}, {2, "c"}};
	REQUIRE(split("a,b,c", ',') == m);
	REQUIRE(handle_newlines("a\nb\nc") == m);

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
	REQUIRE(stringtobool("iowhjoidj") == true);
	REQUIRE(stringtobool("20") == true);

	REQUIRE(booltostring(true) == "true");
	REQUIRE(booltostring(false) == "false");

	REQUIRE(string_replace("a,b,c", ",", ":") == "a:b:c");
	REQUIRE(string_replace("a:test:b:test:c", ":test:", ",") == "a,b,c");

	/*REQUIRE(clipboard_set_text("test") == 0); // These functions require SDL to be initialized
	REQUIRE(clipboard_get_text() == "test");
	REQUIRE(clipboard_has_text() == true);*/
}

// Date and time function assertions
TEST_CASE("date/setters") {
	time_t t = date_create_datetime(2015, 11, 1, 11, 45, 24);
	REQUIRE(date_date_of(t) == date_create_date(2015, 11, 1));
	REQUIRE(date_time_of(t) == date_create_time(11, 45, 24));

	REQUIRE(date_inc_year(t, 5) == date_create_datetime(2020, 11, 1, 11, 45, 24));
	REQUIRE(date_inc_month(t, 5) == date_create_datetime(2015, 16, 1, 11, 45, 24));
	REQUIRE(date_inc_week(t, 5) == date_create_datetime(2015, 11, 36, 11, 45, 24));
	REQUIRE(date_inc_day(t, 5) == date_create_datetime(2015, 11, 6, 11, 45, 24));
	REQUIRE(date_inc_hour(t, 5) == date_create_datetime(2015, 11, 1, 16, 45, 24));
	REQUIRE(date_inc_minute(t, 5) == date_create_datetime(2015, 11, 1, 11, 50, 24));
	REQUIRE(date_inc_second(t, 5) == date_create_datetime(2015, 11, 1, 11, 45, 29));
}
TEST_CASE("date/getters") {
	time_t t = date_create_datetime(2015, 11, 1, 11, 45, 24);
	REQUIRE(date_get_year(t) == 2015);
	REQUIRE(date_get_month(t) == 11);
	REQUIRE(date_get_week(t) == 44);
	REQUIRE(date_get_day(t) == 1);
	REQUIRE(date_get_hour(t) == 11);
	REQUIRE(date_get_minute(t) == 45);
	REQUIRE(date_get_second(t) == 24);
	REQUIRE(date_get_weekday(t) == 0);
	REQUIRE(date_get_day_of_year(t) == 304);
	REQUIRE(date_get_hour_of_year(t) == 7307);
	REQUIRE(date_get_minute_of_year(t) == 438465);
	REQUIRE(date_get_second_of_year(t) == 26307924);

	time_t ot = date_create_datetime(1997, 1, 24, 4, 25, 00);
	REQUIRE(date_year_span(t, ot) == doctest::Approx(18.78).epsilon(0.01));
	REQUIRE(date_month_span(t, ot) == doctest::Approx(228.51).epsilon(0.01));
	REQUIRE(date_week_span(t, ot) == doctest::Approx(979.33).epsilon(0.01));
	REQUIRE(date_day_span(t, ot) == doctest::Approx(6855.31).epsilon(0.01));
	REQUIRE(date_hour_span(t, ot) == doctest::Approx(164527.34).epsilon(0.01));
	REQUIRE(date_minute_span(t, ot) == doctest::Approx(9871640.40).epsilon(0.01));
	REQUIRE(date_second_span(t, ot) == doctest::Approx(592298424.00).epsilon(0.01));
	REQUIRE(date_compare_datetime(t, ot) == 1);
	REQUIRE(date_compare_date(t, ot) == 1);
	REQUIRE(date_compare_time(t, ot) == 1);
	REQUIRE(date_datetime_string(t) == "Sun Nov 01 11:45:24 2015");
	REQUIRE(date_date_string(t) == "Sun Nov 01 2015");
	REQUIRE(date_time_string(t) == "11:45:24");
	REQUIRE(date_is_leap_year(t) == false);
	REQUIRE(date_is_leap_year() == true);
	REQUIRE(date_is_today(t) == false);
	REQUIRE(date_is_today(date_current_datetime()) == true);
	REQUIRE(date_days_in_month(t) == 30);
	REQUIRE(date_days_in_month(ot) == 31);
	REQUIRE(date_days_in_year(t) == 365);
}

// Collision checking function assertions, TODO: add more tests for each case
TEST_CASE("collision/rect") {
	SDL_Rect a = {0, 0, 10, 10};
	SDL_Rect b = {5, 5, 10, 20};
	REQUIRE(check_collision(a, b) == true);
}
TEST_CASE("collision/circle") {
	REQUIRE(check_collision_circle(0, 0, 5, 0, 7, 3) == true);
}
TEST_CASE("collision/line") {
	Line l1 = {0, 0, 5, 10};
	Line l2 = {5, 5, 0, 10};
	REQUIRE(check_collision_line(l1, l2) == true);

	l1 = {0, 0, 10, 0};
	l2 = {5, 5, 5, -5};
	REQUIRE(check_collision_line(l1, l2) == true);
}
TEST_CASE("collision/bounce") {
	REQUIRE(angle_hbounce(60.0) == 120.0);
	REQUIRE(angle_hbounce(120.0) == 60.0);

	REQUIRE(angle_vbounce(60.0) == 300.0);
	REQUIRE(angle_vbounce(300.0) == 60.0);
}

// Message box function assertions
TEST_CASE("messagebox/general") {
	// Right now the message box functions can't be tested because they create a modal dialog
}

// File handling function assertions
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

bool verify_assertions(int argc, char** argv) {
	return !(bool)doctest::Context(argc, argv).run();
}

#else // NDEBUG

bool verify_assertions(int argc, char** argv) {
	return true;
}

#endif // NDEBUG

bool verify_assertions() {
	return verify_assertions(0, (char**)nullptr);
}

#endif // _BEE_UTIL
