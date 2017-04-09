/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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

#ifndef NDEBUG // Allow the NDEBUG to disable debug support at compile time

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h" // Include the required unit testing library which is a git submodule in lib/doctest

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

	REQUIRE(bt_to_glm3(btVector3(0.0, 1.0, 2.0)) == glm::vec3(0.0, 1.0, 2.0));
	REQUIRE(glm_to_bt3(glm::vec3(0.0, 1.0, 2.0)) == btVector3(0.0, 1.0, 2.0));
	REQUIRE(bt_to_glm3(glm_to_bt3(glm::vec3(0.0, 1.0, 2.0))) == glm::vec3(0.0, 1.0, 2.0));
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
	Uint8 ca1[] = {3, 65, 66, 67};
	REQUIRE(chra(ca1) == std::string("ABC"));
	Uint8* ca2 = orda("ABC");
	REQUIRE(chra(ca2) == chra(ca1));
	delete[] ca2;
}
TEST_CASE("string/alteration") {
	REQUIRE(string_lower("ABC") == "abc");
	REQUIRE(string_upper("abc") == "ABC");
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

// Date and time function assertions
TEST_CASE("date/setters") {
	time_t t = date_create_datetime(2015, 11, 1, 11, 45, 24);
	REQUIRE(date_date_of(t) == date_create_date(2015, 11, 1));
	REQUIRE(date_time_of(t) == date_create_time(11, 45, 24));

	REQUIRE(date_inc_year(t, 5) == date_create_datetime(2020, 11, 1, 11, 45, 24));
	REQUIRE(date_inc_month(t, 5) == date_create_datetime(2015, 16, 1, 11, 45, 24));
	//REQUIRE(date_inc_month(t, 5) == date_create_datetime(2016, 4, 1, 11, 45, 24));
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
	REQUIRE(date_compare_datetime(t, ot) == -1);
	REQUIRE(date_compare_date(t, ot) == -1);
	REQUIRE(date_compare_time(t, ot) == -1);
	REQUIRE(date_datetime_string(t) == "Sun Nov 01 11:45:24 2015");
	REQUIRE(date_date_string(t) == "Sun Nov 01 2015");
	REQUIRE(date_time_string(t) == "11:45:24");
	REQUIRE(date_is_leap_year(t) == false);
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
	// Right now the message box functions cannot be tested without displaying the message boxes
	/*REQUIRE(show_message("Press Button 1", "Button 1", "Button 2", "Button 3") == 0);
	REQUIRE(show_message("Press Button 2", "Button 1", "Button 2", "Button 3") == 1);
	REQUIRE(show_message("Press Button 3", "Button 1", "Button 2", "Button 3") == 2);
	REQUIRE(show_message("Press Button 2", "Button 1", "Button 2", "") == 1);
	REQUIRE(show_message("Press Button 1", "Button 1", "", "") == 0);
	REQUIRE(show_question("Press Yes") == true);
	REQUIRE(show_question("Press No") == false);
	REQUIRE(show_message("Press OK") == 0);
	REQUIRE(show_warning("Press OK") == 0);
	REQUIRE(show_error("Press OK") == 0);
	//REQUIRE(show_message("Press the Escape key") == -1);*/
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

// Networking function assertions
TEST_CASE("network") {
	REQUIRE(network_init() == 0);
	int port = 3054;
	IPaddress* ipa = network_resolve_host("127.0.0.1", port);
	REQUIRE(ipa != nullptr);
	REQUIRE(network_get_address(ipa->host) == "127.0.0.1");
	delete ipa;

	TCPsocket tcp = network_tcp_open("", port);
	REQUIRE(tcp != (TCPsocket)nullptr);
	REQUIRE(network_tcp_close(&tcp) == 0);
	REQUIRE(tcp == (TCPsocket)nullptr);

	UDPsocket udp = network_udp_open(port);
	REQUIRE(udp != (UDPsocket)nullptr);
	REQUIRE(network_udp_close(&udp) == 0);
	REQUIRE(udp == (UDPsocket)nullptr);
	REQUIRE(network_udp_bind(&udp, -1, "127.0.0.1", port) != -1);
	REQUIRE(network_udp_unbind(&udp, -1) == 0);

	REQUIRE(network_close() == 0);
}

// Template-requiring function assertions
TEST_CASE("template") {
	int c1 = choose<int>(1, 2, 3, 4);
	REQUIRE(((c1 == 1)||(c1 == 2)||(c1 == 3)||(c1 == 4)));
	double c2 = choose<double>(1.0, 2.0, 3.0, 4.0);
	REQUIRE(((c2 == 1.0)||(c2 == 2.0)||(c2 == 3.0)||(c2 == 4.0)));
	std::string c3 = choose<std::string>("a", "b", "c", "d");
	REQUIRE(((c3 == "a")||(c3 == "b")||(c3 == "c")||(c3 == "d")));

	int min1 = min<int>(1, 2, 3, 4);
	REQUIRE(min1 == 1);
	double min2 = min<double>(1.0, 2.0, 3.0, 4.0);
	REQUIRE(min2 == 1.0);
	std::string min3 = min<std::string>("a", "b", "c", "d");
	REQUIRE(min3 == "a");

	int max1 = max<int>(1, 2, 3, 4);
	REQUIRE(max1 == 4);
	double max2 = max<double>(1.0, 2.0, 3.0, 4.0);
	REQUIRE(max2 == 4.0);
	std::string max3 = max<std::string>("a", "b", "c", "d");
	REQUIRE(max3 == "d");

	int mean1 = mean<int>(1, 2, 3, 4);
	REQUIRE(mean1 == 2);
	double mean2 = mean<double>(1.0, 2.0, 3.0, 4.0);
	REQUIRE(mean2 == 2.5);

	int median1 = median<int>(1, 2, 3, 4);
	REQUIRE(median1 == 2);
	int median2 = median<int>(1, 2, 3, 4, 5);
	REQUIRE(median2 == 3);
	double median3 = median<double>(1.0, 2.0, 3.0, 4.0);
	REQUIRE(median3 == 2.5);
	double median4 = median<double>(1.0, 2.0, 3.0, 4.0, 5.0);
	REQUIRE(median4 == 3.0);

	std::map<std::string, int> m = {
		{ "a", 1 },
		{ "b", 2 },
		{ "c", 3 },
		{ "d", 4 }
	};
	std::map<std::string, int> n;
	Uint8* a = network_map_encode(m);
	REQUIRE(a[0] == 24);
	REQUIRE((network_map_decode(a, &n) == 0));
	delete[] a;
	REQUIRE(std::equal(m.begin(), m.end(), n.begin()));
}

/*
* verify_assertions() - Run the doctest unit tests to verify that all utility functions are working correctly
* @argc: the argc from main()
* @argv: the argv from main()
*/
bool verify_assertions(int argc, char** argv) {
	return !(bool)doctest::Context(argc, argv).run();
}

#else // NDEBUG

/*
* verify_assertions() - If unit testing is disabled, simply return true
*/
bool verify_assertions(int argc, char** argv) {
	return true;
}

#endif // NDEBUG

/*
* verify_assertions() - Run the doctest unit tests to verify that all utility functions are working correctly
* ! When the main() arguments are not provided, simply call the function with an empty set
*/
bool verify_assertions() {
	return verify_assertions(0, (char**)nullptr);
}

#endif // _BEE_UTIL
