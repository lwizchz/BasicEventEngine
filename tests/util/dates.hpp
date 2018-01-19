/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_DATES
#define TESTS_UTIL_DATES 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/dates.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("dates/setters") {
	time_t t = util::date::create_datetime(2015, 11, 1, 11, 45, 24);
	REQUIRE(util::date::date_of(t) == util::date::create_date(2015, 11, 1));
	REQUIRE(util::date::time_of(t) == util::date::create_time(11, 45, 24));

	REQUIRE(util::date::inc_year(t, 5) == util::date::create_datetime(2020, 11, 1, 11, 45, 24));
	REQUIRE(util::date::inc_month(t, 5) == util::date::create_datetime(2015, 16, 1, 11, 45, 24));
	//REQUIRE(util::date::inc_month(t, 5) == util::date::create_datetime(2016, 4, 1, 11, 45, 24));
	REQUIRE(util::date::inc_week(t, 5) == util::date::create_datetime(2015, 11, 36, 11, 45, 24));
	REQUIRE(util::date::inc_day(t, 5) == util::date::create_datetime(2015, 11, 6, 11, 45, 24));
	REQUIRE(util::date::inc_hour(t, 5) == util::date::create_datetime(2015, 11, 1, 16, 45, 24));
	REQUIRE(util::date::inc_minute(t, 5) == util::date::create_datetime(2015, 11, 1, 11, 50, 24));
	REQUIRE(util::date::inc_second(t, 5) == util::date::create_datetime(2015, 11, 1, 11, 45, 29));
}
TEST_CASE("dates/getters") {
	time_t t = util::date::create_datetime(2015, 11, 1, 11, 45, 24);
	REQUIRE(util::date::get_year(t) == 2015);
	REQUIRE(util::date::get_month(t) == 11);
	REQUIRE(util::date::get_week(t) == 44);
	REQUIRE(util::date::get_day(t) == 1);
	REQUIRE(util::date::get_hour(t) == 11);
	REQUIRE(util::date::get_minute(t) == 45);
	REQUIRE(util::date::get_second(t) == 24);
	REQUIRE(util::date::get_weekday(t) == 0);
	REQUIRE(util::date::get_day_of_year(t) == 304);
	REQUIRE(util::date::get_hour_of_year(t) == 7307);
	REQUIRE(util::date::get_minute_of_year(t) == 438465);
	REQUIRE(util::date::get_second_of_year(t) == 26307924);

	time_t ot = util::date::create_datetime(1997, 1, 24, 4, 25, 00);
	REQUIRE(util::date::year_span(t, ot) == doctest::Approx(18.78).epsilon(0.01));
	REQUIRE(util::date::month_span(t, ot) == doctest::Approx(228.51).epsilon(0.01));
	REQUIRE(util::date::week_span(t, ot) == doctest::Approx(979.33).epsilon(0.01));
	REQUIRE(util::date::day_span(t, ot) == doctest::Approx(6855.31).epsilon(0.01));
	REQUIRE(util::date::hour_span(t, ot) == doctest::Approx(164527.34).epsilon(0.01));
	REQUIRE(util::date::minute_span(t, ot) == doctest::Approx(9871640.40).epsilon(0.01));
	REQUIRE(util::date::second_span(t, ot) == doctest::Approx(592298424.00).epsilon(0.01));
	REQUIRE(util::date::compare_datetime(t, ot) == -1);
	REQUIRE(util::date::compare_date(t, ot) == -1);
	REQUIRE(util::date::compare_time(t, ot) == -1);
	REQUIRE(util::date::datetime_string(t) == "Sun Nov 01 11:45:24 2015");
	REQUIRE(util::date::date_string(t) == "Sun Nov 01 2015");
	REQUIRE(util::date::time_string(t) == "11:45:24");
	REQUIRE(util::date::is_leap_year(t) == false);
	REQUIRE(util::date::is_today(t) == false);
	REQUIRE(util::date::is_today(util::date::current_datetime()) == true);
	REQUIRE(util::date::days_in_month(t) == 30);
	REQUIRE(util::date::days_in_month(ot) == 31);
	REQUIRE(util::date::days_in_year(t) == 365);
}

TEST_SUITE_END();

#endif // TESTS_UTIL_DATES
