/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_TEST
#define TESTS_TEST 1

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h" // Include the required unit testing library which is a git submodule in lib/doctest

#include "test.hpp" // Include the function declarations

#include "../bee/messenger/messenger.hpp"

#include "all.hpp"

/**
* Run the doctest unit tests to verify that all utility functions are working correctly.
* @param argc the argc from main()
* @param argv the argv from main()
*
* @returns whether the assertions ran successfully or not
*/
bool verify_assertions(int argc, char** argv) {
	bee::messenger::send({"tests"}, bee::E_MESSAGE::INFO, "Verifying assertions...");
	bee::messenger::handle();
	return (doctest::Context(argc, argv).run() == 0);
}

/**
* Run the doctest unit tests to verify that all utility functions are working correctly.
* @note If the main() arguments are not provided, they will be left empty.
*
* @returns whether the assertions ran successfully or not
*/
bool verify_assertions() {
	return verify_assertions(0, static_cast<char**>(nullptr));
}

#endif // TESTS_TEST
