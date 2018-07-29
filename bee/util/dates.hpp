/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_DATES_H
#define BEE_UTIL_DATES_H 1

#include <string> // Include the required library headers
#include <ctime>
#include <functional>

namespace util { namespace date {
	time_t date_of(time_t);
	time_t time_of(time_t);

	time_t current_datetime();
	time_t current_date();
	time_t current_time();

	time_t create_datetime(int, int, int, int, int, int);
	time_t create_date(int, int, int);
	time_t create_time(int, int, int);

	time_t inc_year(time_t, int);
	time_t inc_month(time_t, int);
	time_t inc_week(time_t, int);
	time_t inc_day(time_t, int);
	time_t inc_hour(time_t, int);
	time_t inc_minute(time_t, int);
	time_t inc_second(time_t, int);

	int get_year(time_t);
	int get_month(time_t);
	int get_week(time_t);
	int get_day(time_t);
	int get_hour(time_t);
	int get_minute(time_t);
	int get_second(time_t);
	int get_weekday(time_t);
	int get_day_of_year(time_t);
	int get_hour_of_year(time_t);
	int get_minute_of_year(time_t);
	int get_second_of_year(time_t);

	double year_span(time_t, time_t);
	double month_span(time_t, time_t);
	double week_span(time_t, time_t);
	double day_span(time_t, time_t);
	double hour_span(time_t, time_t);
	double minute_span(time_t, time_t);
	double second_span(time_t, time_t);

	int compare_datetime(time_t, time_t);
	int compare_date(time_t, time_t);
	int compare_time(time_t, time_t);

	std::string datetime_string(time_t);
	std::string date_string(time_t);
	std::string time_string(time_t);

	bool is_leap_year(time_t);
	bool is_leap_year();
	bool is_today(time_t);

	int days_in_month(time_t);
	int days_in_year(time_t);
}
	double stopwatch(const std::string&, std::function<void()>, bool);
}

#endif // BEE_UTIL_DATES_H
