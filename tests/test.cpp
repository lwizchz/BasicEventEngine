/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_TEST
#define TESTS_TEST 1

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h" // Include the required unit testing library which is a git submodule in lib/doctest

#include "test.hpp" // Include the function declarations

#include "../bee/messenger/messenger.hpp"

#include "all.hpp"

/*
* verify_assertions() - Run the doctest unit tests to verify that all utility functions are working correctly
* @argc: the argc from main()
* @argv: the argv from main()
*/
bool verify_assertions(int argc, char** argv) {
	bee::messenger::send({"tests"}, bee::E_MESSAGE::INFO, "Verifying assertions...");
	bee::messenger::handle();
	return (doctest::Context(argc, argv).run() == 0);
}

/*
* verify_assertions() - Run the doctest unit tests to verify that all utility functions are working correctly
* ! When the main() arguments are not provided, simply call the function with an empty set
*/
bool verify_assertions() {
	return verify_assertions(0, static_cast<char**>(nullptr));
}

#endif // TESTS_TEST
