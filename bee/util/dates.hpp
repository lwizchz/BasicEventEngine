/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_DATES_H
#define BEE_UTIL_DATES_H 1

#include <string> // Include the required library headers
#include <ctime>
#include <functional>

time_t date_date_of(time_t);
time_t date_time_of(time_t);

time_t date_current_datetime();
time_t date_current_date();
time_t date_current_time();

time_t date_create_datetime(int, int, int, int, int, int);
time_t date_create_date(int, int, int);
time_t date_create_time(int, int, int);

time_t date_inc_year(time_t, int);
time_t date_inc_month(time_t, int);
time_t date_inc_week(time_t, int);
time_t date_inc_day(time_t, int);
time_t date_inc_hour(time_t, int);
time_t date_inc_minute(time_t, int);
time_t date_inc_second(time_t, int);

int date_get_year(time_t);
int date_get_month(time_t);
int date_get_week(time_t);
int date_get_day(time_t);
int date_get_hour(time_t);
int date_get_minute(time_t);
int date_get_second(time_t);
int date_get_weekday(time_t);
int date_get_day_of_year(time_t);
int date_get_hour_of_year(time_t);
int date_get_minute_of_year(time_t);
int date_get_second_of_year(time_t);

double date_year_span(time_t, time_t);
double date_month_span(time_t, time_t);
double date_week_span(time_t, time_t);
double date_day_span(time_t, time_t);
double date_hour_span(time_t, time_t);
double date_minute_span(time_t, time_t);
double date_second_span(time_t, time_t);

int date_compare_datetime(time_t, time_t);
int date_compare_date(time_t, time_t);
int date_compare_time(time_t, time_t);

std::string date_datetime_string(time_t);
std::string date_date_string(time_t);
std::string date_time_string(time_t);

bool date_is_leap_year(time_t);
bool date_is_leap_year();
bool date_is_today(time_t);

int date_days_in_month(time_t);
int date_days_in_year(time_t);

double stopwatch(std::string, std::function<void()>);

#endif // BEE_UTIL_DATES_H
