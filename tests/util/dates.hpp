/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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
TEST_CASE("dates/getters") {
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

TEST_SUITE_END();

#endif // TESTS_UTIL_DATES
