/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
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

namespace util { namespace date {
/**
* @param date the timestamp to operate on
* @returns a timestamp corresponding to the given timestamp's date
*/
time_t date_of(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	return create_date(timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday); // Return a new timestamp based on the given date
}
/**
* @param date the timestamp to operate on
* @returns a timestamp corresponding to the given timestamp's time
*/
time_t time_of(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	return create_time(timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec); // Return a new timestamp based on the given time
}

/**
* @returns a timestamp corresponding to now
*/
time_t current_datetime() {
	return time(nullptr);
}
/**
* @returns a timestamp corresponding to the date right now
*/
time_t current_date() {
	return date_of(current_datetime());
}
/**
* @returns a timestamp corresponding to the time right now
*/
time_t current_time() {
	return time_of(current_datetime());
}

/**
* @see http://www.cplusplus.com/reference/ctime/tm/ for details
* @param year the new timestamp's year
* @param month the new timestamp's month
* @param day the new timestamp's day
* @param hour the new timestamp's hour
* @param minute the new timestamp's minute
* @param second the new timestamp's second
*
* @returns a timestamp with the given date and time
*/
time_t create_datetime(int year, int month, int day, int hour, int minute, int second) {
	struct tm timeinfo; // Declare a new info struct

	// Fill in the fields of the info struct with the given values
	timeinfo.tm_year = year-1900;
	timeinfo.tm_mon = month-1;
	timeinfo.tm_mday = day;
	timeinfo.tm_hour = hour;
	timeinfo.tm_min = minute;
	timeinfo.tm_sec = second;
	timeinfo.tm_isdst = 0;

	return mktime(&timeinfo); // Convert the struct into a timestamp
}
/**
* @param year the new timestamp's year
* @param month the new timestamp's month
* @param day the new timestamp's day
*
* @returns a timestamp with the given date
*/
time_t create_date(int year, int month, int day) {
	return create_datetime(year, month, day, 0, 0, 0);
}
/**
* @param hour the new timestamp's hour
* @param minute the new timestamp's minute
* @param second the new timestamp's second
*
* @returns a timestamp with the given time
*/
time_t create_time(int hour, int minute, int second) {
	return create_datetime(1970, 1, 1, hour, minute, second);
}

/**
* @param old_date the timestamp to increment
* @param amount the amount by which to increment the timestamp
*
* @returns a timestamp with the given timestamp's year incremented by the given amount
*/
time_t inc_year(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_year += amount;
	return mktime(timeinfo);
}
/**
* @param old_date the timestamp to increment
* @param amount the amount by which to increment the timestamp
*
* @returns a timestamp with the given timestamp's month incremented by the given amount
*/
time_t inc_month(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_mon += amount;
	return mktime(timeinfo);
}
/**
* @param old_date the timestamp to increment
* @param amount the amount by which to increment the timestamp
*
* @returns a timestamp with the given timestamp's week incremented by the given amount
*/
time_t inc_week(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_mday += amount*7;
	return mktime(timeinfo);
}
/**
* @param old_date the timestamp to increment
* @param amount the amount by which to increment the timestamp
*
* @returns a timestamp with the given timestamp's day incremented by the given amount
*/
time_t inc_day(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_mday += amount;
	return mktime(timeinfo);
}
/**
* @param old_date the timestamp to increment
* @param amount the amount by which to increment the timestamp
*
* @returns a timestamp with the given timestamp's hour incremented by the given amount
*/
time_t inc_hour(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_hour += amount;
	return mktime(timeinfo);
}
/**
* @param old_date the timestamp to increment
* @param amount the amount by which to increment the timestamp
*
* @returns a timestamp with the given timestamp's minute incremented by the given amount
*/
time_t inc_minute(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_min += amount;
	return mktime(timeinfo);
}
/**
* @param old_date the timestamp to increment
* @param amount the amount by which to increment the timestamp
*
* @returns a timestamp with the given timestamp's second incremented by the given amount
*/
time_t inc_second(time_t old_date, int amount) {
	struct tm* timeinfo = localtime(&old_date); // Convert the timestamp into an info struct, add the given amount, and return the data as a timestamp
	timeinfo->tm_sec += amount;
	return mktime(timeinfo);
}

/**
* @param date the timestamp to operate on
*
* @returns the year value of the given timestamp
*/
int get_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_year+1900;
}
/**
* @param date the timestamp to operate on
*
* @returns the month value of the given timestamp
*/
int get_month(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_mon+1;
}
/**
* @param date the timestamp to operate on
*
* @returns the week value of the given timestamp
*/
int get_week(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return static_cast<int>(timeinfo->tm_yday / 7) + 1;
}
/**
* @param date the timestamp to operate on
*
* @returns the day value of the given timestamp
*/
int get_day(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_mday;
}
/**
* @param date the timestamp to operate on
*
* @returns the hour value of the given timestamp
*/
int get_hour(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_hour;
}
/**
* @param date the timestamp to operate on
*
* @returns the minute value of the given timestamp
*/
int get_minute(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_min;
}
/**
* @param date the timestamp to operate on
*
* @returns the second value of the given timestamp
*/
int get_second(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_sec;
}
/**
* @param date the timestamp to operate on
*
* @returns the weekday of the given timestamp
*/
int get_weekday(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_wday;
}
/**
* @param date the timestamp to operate on
*
* @returns the day of year of the given timestamp
*/
int get_day_of_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_yday;
}
/**
* @oaram date the timestamp to operate on
*
* @returns the hour of year of the given timestamp
*/
int get_hour_of_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_yday*24 + timeinfo->tm_hour;
}
/**
* @param date the timestamp to operate on
*
* @returns the minute of year of the given timestamp
*/
int get_minute_of_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_yday*24*60 + timeinfo->tm_hour*60 + timeinfo->tm_min;
}
/**
* @param date the timestamp to operate on
*
* @returns the second of year of the given timestamp
*/
int get_second_of_year(time_t date) {
	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct and return the requested value
	return timeinfo->tm_yday*24*60*60 + timeinfo->tm_hour*60*60 + timeinfo->tm_min*60 + timeinfo->tm_sec;
}

/**
* @see http://www.cplusplus.com/reference/ctime/difftime/ for details
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns the difference in years between the two timestamps
*/
double year_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60*24*365);
}
/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns the difference in months between the two timestamps
*/
double month_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60*24*30);
}
/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns the difference in weeks between the two timestamps
*/
double week_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60*24*7);
}
/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns the difference in days between the two timestamps
*/
double day_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60*24);
}
/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns the difference in hours between the two timestamps
*/
double hour_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / (60*60);
}
/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns the difference in minutes between the two timestamps
*/
double minute_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2))) / 60;
}
/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns the difference in seconds between the two timestamps
*/
double second_span(time_t date1, time_t date2) {
	return static_cast<double>(abs(difftime(date1, date2)));
}

/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns an sign value (-1, 0, or 1) depending on the relationship between the two timestamps
* @note -1 indicates that date1 is more recent, 0 indicates that they are equal, and 1 indicates that date2 is more recent.
*/
int compare_datetime(time_t date1, time_t date2) {
	return sign(date2-date1);
}
/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns an sign value (-1, 0, or 1) depending on the relationship between the two dates
* @note -1 indicates that date1 is more recent, 0 indicates that they are equal, and 1 indicates that date2 is more recent.
*/
int compare_date(time_t date1, time_t date2) {
	return compare_datetime(date_of(date1), date_of(date2));
}
/**
* @param date1 one of the timestamps
* @param date2 the other timestamp
*
* @returns an sign value (-1, 0, or 1) depending on the relationship between the two times
* @note -1 indicates that date1 is more recent, 0 indicates that they are equal, and 1 indicates that date2 is more recent.
*/
int compare_time(time_t date1, time_t date2) {
	return compare_datetime(time_of(date1), time_of(date2));
}

/**
* @see http://www.cplusplus.com/reference/ctime/strftime/ for details
* @param date the timestamp to operate on
*
* @returns a string with a human-readable version of the given timestamp
*/
std::string datetime_string(time_t date) {
	char* str = new char[25]; // Allocate space for the formatted string

	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	strftime(str, 25, "%a %b %d %H:%M:%S %Y", timeinfo); // Store a formatted time string in str

	std::string s (str); // Create a string from the character array
	delete[] str; // Free the character array

	return s; // Return the string on success
}
/**
* @param date the timestamp to operate on
*
* @returns a string with a human-readable version of the given date
*/
std::string date_string(time_t date) {
	char* str = new char[16]; // Allocate space for the formatted string
	date = date_of(date); // Get only the date portion of the timestamp

	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	strftime(str, 16, "%a %b %d %Y", timeinfo); // Store the formatted time string in str

	std::string s (str); // Create a string from the character array
	delete[] str; // Free the character array

	return s;
}
/**
* @param date the timestamp to operate on
*
* @returns a string with a human-readable version of the given time
*/
std::string time_string(time_t date) {
	char* str = new char[9]; // Allocate space for the formatted string
	date = time_of(date); // Get only the time portion of the timestamp

	struct tm* timeinfo = localtime(&date); // Convert the timestamp into an info struct
	strftime(str, 9, "%H:%M:%S", timeinfo); // Store the formatted time string in str

	std::string s (str); // Create a string from the character array
	delete[] str; // Free the character array

	return s;
}

/**
* @param date the timestamp to operate on
*
* @returns whether the year of the given timestamp is a leap year or not
*/
bool is_leap_year(time_t date) {
	int y = get_year(date); // Get the date portion of the given timestamp
	if ((y % 4 == 0) && ((y % 100 != 0)||(y % 400 == 0))) { // If the year is divisible by 4 but is not divisble by 100 on non-400 divisble years, then it is a leap year
		return true;
	}
	return false; // Otherwise it is not a leap year
}
/**
* @returns whether the current year is a leap year or not
*/
bool is_leap_year() {
	return is_leap_year(current_datetime());
}
/**
* @param date the timestamp to operate on
*
* @returns whether the date of the given timestamp is today's date
*/
bool is_today(time_t date) {
	if (date_of(date) == current_date()) {
		return true;
	}
	return false;
}

/**
* @param date the timestamp to operate on
*
* @returns how many days are in the month of the given timestamp
*/
int days_in_month(time_t date) {
	switch (get_month(date)) {
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
			if (is_leap_year(date)) {
				return 29;
			}
			return 28;
		}

		default: { // Regular months: April, June, September, November
			return 30;
		}
	}
}
/**
* @param date the timestamp to operate on
*
* @returns how many days are in the year of the given timestamp
*/
int days_in_year(time_t date) {
	if (is_leap_year(date)) { // If it is a leap year, then return the extra number of days
		return 366;
	}
	return 365; // Otherwise return the normal number of days
}

}

/**
* Print out how long it took a function to execute.
* @param name the name of the function to print out
* @param func the function to call
* @param should_print whether to print the elapsed time
*
* @returns the elapsed time of the function
*/
double stopwatch(const std::string& name, std::function<void()> func, bool should_print) {
	auto start = std::chrono::high_resolution_clock::now(); // Store the current time

	func(); // Call the function

	auto end = std::chrono::high_resolution_clock::now(); // Store the new current time
	double elapsed_ms = std::chrono::duration<double,std::milli>(end-start).count(); // Calculate the elapsed time

	if (should_print) {
		std::cout << "UTIL The function \"" << name << "\" finished in " << elapsed_ms << "ms.\n"; // Output the function name and elapsed time
	}

	return elapsed_ms;
}

}

#endif // BEE_UTIL_DATES
