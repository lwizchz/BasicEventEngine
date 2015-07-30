/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_UTIL_DATES_H
#define _BEE_UTIL_DATES_H 1

// Date and time functions

#include <time.h>

time_t date_date_of(time_t date) {
        struct tm *timeinfo = localtime(&date);
        timeinfo->tm_sec = 0;
        timeinfo->tm_min = 0;
        timeinfo->tm_hour = 0;
        timeinfo->tm_isdst = -1;
        return mktime(timeinfo);
}
time_t date_time_of(time_t date) {
        struct tm *timeinfo = localtime(&date);
        timeinfo->tm_mday = 0;
        timeinfo->tm_mon = 0;
        timeinfo->tm_year = 0;
        return mktime(timeinfo);
}

time_t date_current_datetime() {
        return time(NULL);
}
time_t date_current_date() {
        return date_date_of(date_current_datetime());
}
time_t date_current_time() {
        return date_time_of(date_current_datetime());
}

time_t date_create_datetime(int year, int month, int day, int hour, int minute, int second) {
        struct tm timeinfo = {second, minute, hour, day, month, year, 0, 0, -1, -1, ""};
        return mktime(&timeinfo);
}
time_t date_create_date(int year, int month, int day) {
        return date_create_datetime(year, month, day, 0, 0, 0);
}
time_t date_create_time(int hour, int minute, int second) {
        return date_create_datetime(0, 0, 0, hour, minute, second);
}

time_t date_inc_year(time_t old_date, int amount) {
        struct tm *timeinfo = localtime(&old_date);
        timeinfo->tm_year += amount;
        return mktime(timeinfo);
}
time_t date_inc_month(time_t old_date, int amount) {
        struct tm *timeinfo = localtime(&old_date);
        timeinfo->tm_mon += amount;
        return mktime(timeinfo);
}
time_t date_inc_week(time_t old_date, int amount) {
        struct tm *timeinfo = localtime(&old_date);
        timeinfo->tm_mday += amount*7;
        return mktime(timeinfo);
}
time_t date_inc_day(time_t old_date, int amount) {
        struct tm *timeinfo = localtime(&old_date);
        timeinfo->tm_mday += amount;
        return mktime(timeinfo);
}
time_t date_inc_hour(time_t old_date, int amount) {
        struct tm *timeinfo = localtime(&old_date);
        timeinfo->tm_hour += amount;
        return mktime(timeinfo);
}
time_t date_inc_minute(time_t old_date, int amount) {
        struct tm *timeinfo = localtime(&old_date);
        timeinfo->tm_min += amount;
        return mktime(timeinfo);
}
time_t date_inc_second(time_t old_date, int amount) {
        struct tm *timeinfo = localtime(&old_date);
        timeinfo->tm_sec += amount;
        return mktime(timeinfo);
}

int date_get_year(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_year+1900;
}
int date_get_month(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_mon+1;
}
int date_get_week(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return (int)(timeinfo->tm_yday / 7) + 1;
}
int date_get_day(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_mday;
}
int date_get_hour(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_hour;
}
int date_get_minute(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_min;
}
int date_get_second(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_sec;
}
int date_get_weekday(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_wday;
}
int date_get_day_of_year(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_yday;
}
int date_get_hour_of_year(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_yday*24 + timeinfo->tm_hour;
}
int date_get_minute_of_year(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_yday*24*60 + timeinfo->tm_hour*60 + timeinfo->tm_min;
}
int date_get_second_of_year(time_t date) {
        struct tm *timeinfo = localtime(&date);
        return timeinfo->tm_yday*24*60*60 + timeinfo->tm_hour*60*60 + timeinfo->tm_min*60 + timeinfo->tm_sec;
}

double date_year_span(time_t date1, time_t date2) {
        return abs(difftime(date1, date2)) / (60*60*24*365);
}
double date_month_span(time_t date1, time_t date2) {
        return abs(difftime(date1, date2)) / (60*60*24*30);
}
double date_week_span(time_t date1, time_t date2) {
        return abs(difftime(date1, date2)) / (60*60*24*7);
}
double date_day_span(time_t date1, time_t date2) {
        return abs(difftime(date1, date2)) / (60*60*24);
}
double date_hour_span(time_t date1, time_t date2) {
        return abs(difftime(date1, date2)) / (60*60);
}
double date_minute_span(time_t date1, time_t date2) {
        return abs(difftime(date1, date2)) / (60);
}
double date_second_span(time_t date1, time_t date2) {
        return abs(difftime(date1, date2));
}

int date_compare_datetime(time_t date1, time_t date2) {
        return sign(date1-date2);
}
int date_compare_date(time_t date1, time_t date2) {
        date1 = date_date_of(date1);
        date2 = date_date_of(date2);
        return date_compare_datetime(date1, date2);
}
int date_compare_time(time_t date1, time_t date2) {
        date1 = date_time_of(date1);
        date2 = date_time_of(date2);
        return date_compare_datetime(date1, date2);
}

std::string date_datetime_string(time_t date) {
        return ctime(&date);
}
std::string date_date_string(time_t date) {
        date = date_date_of(date);
        return ctime(&date);
}
std::string date_time_string(time_t date) {
        date = date_time_of(date);
        return ctime(&date);
}

bool date_is_leap_year(time_t date) {
        struct tm *timeinfo = localtime(&date);
        int y = timeinfo->tm_year;
        if ((y % 4 == 0) && ((y % 100 != 0)||(y % 400 == 0))) {
                return true;
        }
        return false;
}
bool date_is_leap_year() {
        return date_is_leap_year(time(NULL));
}
bool date_is_today(time_t date) {
        if (date_date_of(date) == date_date_of(date_current_date())) {
                return true;
        }
        return false;
}

int date_days_in_month(time_t date) {
        struct tm *timeinfo = localtime(&date);
        switch (timeinfo->tm_mon) {
                case 0: // January
                case 2: // March
                case 4: // May
                case 6: // July
                case 7: // August
                case 9: // October
                case 11: { // December
                        return 31;
                }
                case 1: { // February
                        if (date_is_leap_year(date)) {
                                return 29;
                        }
                        return 28;
                }
                default: { // Regular months: April, June, September, November
                        return 30;
                }
        }
        return 30;
}
int date_days_in_year(time_t date) {
        if (date_is_leap_year(date)) {
                return 366;
        }
        return 365;
}

#endif // _BEE_UTIL_DATES_H
