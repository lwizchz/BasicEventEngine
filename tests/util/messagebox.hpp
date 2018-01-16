/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_MESSAGEBOX
#define TESTS_UTIL_MESSAGEBOX 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/messagebox.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("messagebox") {
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

TEST_SUITE_END();

#endif // TESTS_UTIL_MESSAGEBOX
