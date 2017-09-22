/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_UTIL_DATES
#define BEE_UTIL_DATES 1

// Date and time functions

#include <iostream>
#include <cmath>
#include <chrono>

#include "dates.hpp" // Include the function definitions

#include "platform.hpp" // Include the required DST Windows functions
#include "real.hpp" // Include the required real number functions

/*
* date_date_of() - Return a timestamp corresponding to the given timestamp's date
* @date: the timestamp to operate on
*/
time_t date_date_of(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	return date_create_date(timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday); // Return a new timestamp based on the given date
}
/*
* date_time_of() - Return a timestamp corresponding to the given timestamp's time
* @date: the timestamp to operate on
*/
time_t date_time_of(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	return date_create_time(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); // Return a new timestamp based on the given time
}

/*
* date_current_datetime() - Return a timestamp corresponding to now
*/
time_t date_current_datetime() {
	return time(nullptr);
}
/*
* date_current_date() - Return a timestamp corresponding to the date right now
*/
time_t date_current_date() {
	return date_date_of(date_current_datetime());
}
/*
* date_current_time() - Return a timestamp corresponding to the time right now
*/
time_t date_current_time() {
	return date_time_of(date_current_datetime());
}

/*
* date_create_datetime() - Return a timestamp with the given date and time
* ! See http://www.cplusplus.com/reference/ctime/tm/ for details
* @year: the new timestamp's year
* @month: the new timestamp's month
* @day: the new timestamp's day
* @hour: the new timestamp's hour
* @minute: the new timestamp's minute
* @second: the new timestamp's second
*/
time_t date_create_datetime(int year, int month, int day, int hour, int minute, int second) {
	struct tm timeinfo; // Declare a new info struct

	// Fill in the fields of the info struct with the given values
	timeinfo.tm_year = year-1900;
	timeinfo.tm_mon = month-1;
	timeinfo.tm_mday = day;
	timeinfo.tm_hour = hour;
	timeinfo.tm_min = minute;
	timeinfo.tm_sec = second;
	timeinfo.tm_isdst = 0;

	return mktime(&timeinfo); // Return a timestamp from the given struct
}
/*
* date_create_date() - Return a timestamp with the given date
* @year: the new timestamp's year
* @month: the new timestamp's month
* @day: the new timestamp's day
*/
time_t date_create_date(int year, int month, int day) {
	return date_create_datetime(year, month, day, 0, 0, 0);
}
/*
* date_create_time() - Return a timestamp with the given time
* @hour: the new timestamp's hour
* @minute: the new timestamp's minute
* @second: the new timestamp's second
*/
time_t date_create_time(int hour, int minute, int second) {
	return date_create_datetime(1970, 1, 1, hour, minute, second);
}

/*
* date_inc_year() - Return a timestamp with the given timestamp's year incremented by the given amount
* @old_date: the timestamp to increment
* @amount: the amount by which to increment the timestamp
*/
time_t date_inc_year(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_year += amount;
	return mktime(timeinfo);
}
/*
* date_inc_month() - Return a timestamp with the given timestamp's month incremented by the given amount
* @old_date: the timestamp to increment
* @amount: the amount by which to increment the timestamp
*/
time_t date_inc_month(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_mon += amount;
	return mktime(timeinfo);
}
/*
* date_inc_week() - Return a timestamp with the given timestamp's week incremented by the given amount
* @old_date: the timestamp to increment
* @amount: the amount by which to increment the timestamp
*/
time_t date_inc_week(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_mday += amount*7;
	return mktime(timeinfo);
}
/*
* date_inc_day() - Return a timestamp with the given timestamp's day incremented by the given amount
* @old_date: the timestamp to increment
* @amount: the amount by which to increment the timestamp
*/
time_t date_inc_day(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_mday += amount;
	return mktime(timeinfo);
}
/*
* date_inc_hour() - Return a timestamp with the given timestamp's hour incremented by the given amount
* @old_date: the timestamp to increment
* @amount: the amount by which to increment the timestamp
*/
time_t date_inc_hour(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_hour += amount;
	return mktime(timeinfo);
}
/*
* date_inc_minute() - Return a timestamp with the given timestamp's minute incremented by the given amount
* @old_date: the timestamp to increment
* @amount: the amount by which to increment the timestamp
*/
time_t date_inc_minute(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_min += amount;
	return mktime(timeinfo);
}
/*
* date_inc_second() - Return a timestamp with the given timestamp's second incremented by the given amount
* @old_date: the timestamp to increment
* @amount: the amount by which to increment the timestamp
*/
time_t date_inc_second(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_sec += amount;
	return mktime(timeinfo);
}

/*
* date_get_year() - Return the year value of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_year+1900;
}
/*
* date_get_month() - Return the month value of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_month(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_mon+1;
}
/*
* date_get_week() - Return the week value of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_week(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return static_cast<int>(timeinfo->tm_yday / 7) + 1;
}
/*
* date_get_day() - Return the day value of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_day(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_mday;
}
/*
* date_get_hour() - Return the hour value of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_hour(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_hour;
}
/*
* date_get_minute() - Return the minute value of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_minute(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_min;
}
/*
* date_get_second() - Return the second value of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_second(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_sec;
}
/*
* date_get_weekday() - Return the weekday of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_weekday(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_wday;
}
/*
* date_get_day_of_year() - Return the day of year of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_day_of_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_yday;
}
/*
* date_get_hour_of_year() - Return the hour of year of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_hour_of_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_yday*24 + timeinfo->tm_hour;
}
/*
* date_get_minute_of_year() - Return the minute of year of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_minute_of_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_yday*24*60 + timeinfo->tm_hour*60 + timeinfo->tm_min;
}
/*
* date_get_second_of_year() - Return the second of year of the given timestamp
* @date: the timestamp to operate on
*/
int date_get_second_of_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_yday*24*60*60 + timeinfo->tm_hour*60*60 + timeinfo->tm_min*60 + timeinfo->tm_sec;
}

/*
* date_year_span() - Return the difference in years between the two timestamps
* ! See http://www.cplusplus.com/reference/ctime/difftime/ for details
* @date1: one of the timestamps
* @date2: the other timestamp
*/
double date_year_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60*24*365);
}
/*
* date_month_span() - Return the difference in months between the two timestamps
* @date1: one of the timestamps
* @date2: the other timestamp
*/
double date_month_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60*24*30);
}
/*
* date_week_span() - Return the difference in weeks between the two timestamps
* @date1: one of the timestamps
* @date2: the other timestamp
*/
double date_week_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60*24*7);
}
/*
* date_day_span() - Return the difference in days between the two timestamps
* @date1: one of the timestamps
* @date2: the other timestamp
*/
double date_day_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60*24);
}
/*
* date_hour_span() - Return the difference in hours between the two timestamps
* @date1: one of the timestamps
* @date2: the other timestamp
*/
double date_hour_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60);
}
/*
* date_minute_span() - Return the difference in minutes between the two timestamps
* @date1: one of the timestamps
* @date2: the other timestamp
*/
double date_minute_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / 60;
}
/*
* date_seconds_span() - Return the difference in seconds between the two timestamps
* @date1: one of the timestamps
* @date2: the other timestamp
*/
double date_second_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2)));
}

/*
* date_compare_datetime() - Return an sign value (-1, 0, or 1) depending on the relationship between the two timestamps
* ! -1 indicates that date1 is more recent, 0 indicates that they are equal, and 1 indicates that date2 is more recent
* @date1: one of the timestamps
* @date2: the other timestamp
*/
int date_compare_datetime(time_t date1, time_t date2) {
	return sign(date2-date1);
}
/*
* date_compare_date() - Return an sign value (-1, 0, or 1) depending on the relationship between the two dates
* ! -1 indicates that date1 is more recent, 0 indicates that they are equal, and 1 indicates that date2 is more recent
* @date1: one of the timestamps
* @date2: the other timestamp
*/
int date_compare_date(time_t date1, time_t date2) {
	return date_compare_datetime(date_date_of(date1), date_date_of(date2));
}
/*
* date_compare_time() - Return an sign value (-1, 0, or 1) depending on the relationship between the two times
* ! -1 indicates that date1 is more recent, 0 indicates that they are equal, and 1 indicates that date2 is more recent
* @date1: one of the timestamps
* @date2: the other timestamp
*/
int date_compare_time(time_t date1, time_t date2) {
	return date_compare_datetime(date_time_of(date1), date_time_of(date2));
}

/*
* date_datetime_string() - Return a string with a human-readable version of the given timestamp
* ! See http://www.cplusplus.com/reference/ctime/strftime/ for details
* @date: the timestamp to operate on
*/
std::string date_datetime_string(time_t date) {
	char* str = new char[25]; // Allocate space for the formatted string

	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	strftime(str, 25, "%a %b %d %H:%M:%S %Y", timeinfo); // Store a formatted time string in str

	std::string s (str); // Create a string from the character array
	delete[] str; // Free the character array

	return s; // Return the string on success
}
/*
* date_date_string() - Return a string with a human-readable version of the given date
* @date: the timestamp to operate on
*/
std::string date_date_string(time_t date) {
	char* str = new char[16]; // Allocate space for the formatted string
	date = date_date_of(date); // Get only the date portion of the timestamp

	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	strftime(str, 16, "%a %b %d %Y", timeinfo); // Store the formatted time string in str

	std::string s (str); // Create a string from the character array
	delete[] str; // Free the character array

	return s; // Return the string on success
}
/*
* date_time_string() - Return a string with a human-readable version of the given time
* @date: the timestamp to operate on
*/
std::string date_time_string(time_t date) {
	char* str = new char[9]; // Allocate space for the formatted string
	date = date_time_of(date); // Get only the time portion of the timestamp

	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	strftime(str, 9, "%H:%M:%S", timeinfo); // Store the formatted time string in str

	std::string s (str); // Create a string from the character array
	delete[] str; // Free the character array

	return s; // Return the string on success
}

/*
* date_is_leap_year() - Return whether the year of the given timestamp is a leap year or not
* @date: the timestamp to operate on
*/
bool date_is_leap_year(time_t date) {
	int y = date_get_year(date); // Get the date portion of the given timestamp
	if ((y % 4 == 0) && ((y % 100 != 0)||(y % 400 == 0))) { // If the year is divisible by 4 but is not divisble by 100 on non-400 divisble years, then it is a leap year
		return true;
	}
	return false; // Otherwise it is not a leap year
}
/*
* date_is_leap_year() - Return whether the current year is a leap year or not
*/
bool date_is_leap_year() {
	return date_is_leap_year(date_current_datetime());
}
/*
* date_is_today() - Return whether the date of the given timestamp is today's date
* @date: the timestamp to operate on
*/
bool date_is_today(time_t date) {
	if (date_date_of(date) == date_current_date()) {
		return true;
	}
	return false;
}

/*
* date_days_in_month() - Return how many days are in the month of the given timestamp
* @date: the timestamp to operate on
*/
int date_days_in_month(time_t date) {
	switch (date_get_month(date)) {
		case 1: // January
		case 3: // March
		case 5: // May
		case 7: // July
		case 8: // August
		case 10: // October
		case 12: { // December
			return 31;
		}

		case 2: { // February
			if (date_is_leap_year(date)) {
				return 29;
			}
			return 28;
		}

		default: { // Regular months: April, June, September, November
			return 30;
		}
	}
	return 30; // This should never happen
}
/*
* date_days_in_year() - Return how many days are in the year of the given timestamp
* @date: the timestamp to operate on
*/
int date_days_in_year(time_t date) {
	if (date_is_leap_year(date)) { // If it is a leap year, then return the extra number of days
		return 366;
	}
	return 365; // Otherwise return the normal number of days
}

/*
* stopwatch() - Print out how long it took a function to execute
* @name: the name of the function to print out
* @func: the function to call
*/
double stopwatch(std::string name, std::function<void()> func) {
	auto start = std::chrono::high_resolution_clock::now(); // Store the current time

	func(); // Call the function

	auto end = std::chrono::high_resolution_clock::now(); // Store the new current time
	double elapsed_ms = std::chrono::duration<double,std::milli>(end-start).count(); // Calculate the elapsed time

	std::cout << "UTIL The function \"" << name << "\" finished in " << elapsed_ms << "ms.\n"; // Output the function name and elapsed time

	return elapsed_ms; // Return the elapsed time
}

#endif // BEE_UTIL_DATES
