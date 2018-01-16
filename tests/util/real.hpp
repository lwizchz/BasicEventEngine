/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_UTIL_REAL
#define TESTS_UTIL_REAL 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/util/real.hpp"

TEST_SUITE_BEGIN("util");

TEST_CASE("real/random") {
	REQUIRE(random(20) < static_cast<unsigned int>(20));
	REQUIRE(is_between(static_cast<int>(random_range(20, 40)), 20, 40));
	REQUIRE(random_set_seed(5) == static_cast<unsigned int>(5));
	REQUIRE(random_reset_seed() != static_cast<unsigned int>(DEFAULT_RANDOM_SEED));
	REQUIRE(randomize() != static_cast<unsigned int>(DEFAULT_RANDOM_SEED));
}
TEST_CASE("real/random") {
	REQUIRE(random(20) < static_cast<unsigned int>(20));
	REQUIRE(is_between(static_cast<int>(random_range(20, 40)), 20, 40));
	REQUIRE(random_set_seed(5) == static_cast<unsigned int>(5));
	REQUIRE(random_reset_seed() != static_cast<unsigned int>(DEFAULT_RANDOM_SEED));
	REQUIRE(randomize() != static_cast<unsigned int>(DEFAULT_RANDOM_SEED));
}
TEST_CASE("real/math") {
	REQUIRE(sign(5) == 1);
	REQUIRE(sign(0) == 0);
	REQUIRE(sign(-5) == -1);
	REQUIRE(sign(5.0) == 1);
	REQUIRE(sign(0.0) == 0);
	REQUIRE(sign(-5.0) == -1);
	REQUIRE(sqr(5) == 25);
	REQUIRE(sqr(5.0) == 25.0);
	REQUIRE(logn(5.0, 1.0) == 0.0);
	REQUIRE(logn(5.0, 5.0) == doctest::Approx(1.0));
	REQUIRE(logn(5.0, 10.0) == doctest::Approx(1.431).epsilon(0.001));
	REQUIRE(degtorad(90.0) == doctest::Approx(PI/2.0));
	REQUIRE(degtorad(360.0) == doctest::Approx(2.0*PI));
	REQUIRE(radtodeg(PI) == doctest::Approx(180.0));
	REQUIRE(radtodeg(PI/3.0) == doctest::Approx(60.0));
	REQUIRE(opposite_angle(0.0) == 180.0);
	REQUIRE(opposite_angle(60.0) == 240.0);
	REQUIRE(opposite_angle(270.0) == 90.0);
	REQUIRE(opposite_angle(360.0) == 180.0);
	REQUIRE(opposite_angle(-90.0) == 90.0);
}
TEST_CASE("real/movement") {
	REQUIRE(direction_of(1.0, 2.0, 3.0, 4.0) == doctest::Approx(opposite_angle(direction_of(3.0, 4.0, 1.0, 2.0))));
	REQUIRE(direction_of(0.0, 0.0, 1.0, 0.0) == doctest::Approx(0.0));
	REQUIRE(direction_of(0.0, 0.0, 0.0, 1.0) == doctest::Approx(90.0));
	REQUIRE(direction_of(0.0, 0.0, -1.0, 0.0) == doctest::Approx(180.0));
	REQUIRE(direction_of(0.0, 0.0, 0.0, -1.0) == doctest::Approx(270.0));
	REQUIRE(distance(0.0, 0.0, 1.0, 0.0) == 1.0);
	REQUIRE(distance(0.0, 0.0, 3.0, 4.0) == 5.0);
	REQUIRE(distance(0.0, 0.0, 0.0, 0.0) == 0.0);

	REQUIRE(bt_to_glm3(btVector3(0.0, 1.0, 2.0)) == glm::vec3(0.0, 1.0, 2.0));
	REQUIRE(glm_to_bt3(glm::vec3(0.0, 1.0, 2.0)) == btVector3(0.0, 1.0, 2.0));
	REQUIRE(bt_to_glm3(glm_to_bt3(glm::vec3(0.0, 1.0, 2.0))) == glm::vec3(0.0, 1.0, 2.0));
}
TEST_CASE("real/dot_product") {
	REQUIRE(dot_product(1, 2, 3, 4) == 11);
	REQUIRE(dot_product(1.0, 2.0, 3.0, 4.0) == 11.0);
	REQUIRE(dot_product(std::make_pair(1, 2), std::make_pair(3, 4)) == 11);
}
TEST_CASE("real/bounds") {
	REQUIRE(is_between(5, 3, 6) == true);
	REQUIRE(is_between(6, 3, 6) == true);
	REQUIRE(is_between(7, 3, 6) == false);
	REQUIRE(is_between(5, 6, 3) == true);
	REQUIRE(is_between(5.0, 3.0, 6.0) == true);
	REQUIRE(is_angle_between(90, 0, 180) == true);
	REQUIRE(is_angle_between(180, 0, 180) == true);
	REQUIRE(is_angle_between(270, 0, 180) == false);
	REQUIRE(is_angle_between(90.0, 0.0, 180.0) == true);
	REQUIRE(is_angle_between(0, 270, 90) == true);
	REQUIRE(is_angle_between(360, 270, 90) == true);
	REQUIRE(fit_bounds(5, 3, 6) == 5);
	REQUIRE(fit_bounds(6, 3, 6) == 6);
	REQUIRE(fit_bounds(7, 3, 6) == 6);
	REQUIRE(fit_bounds(5, 6, 3) == 5);
	REQUIRE(fit_bounds(5.0, 3.0, 6.0) == 5.0);
	REQUIRE(qmod(5, 3) == 2);
	REQUIRE(qmod(3, 3) == 0);
	REQUIRE(qmod(2, 3) == 2);
	REQUIRE(qmod(5.f, 3) == 2.f);
	REQUIRE(qmod(5.f, 3) == fmod(5.f, 3.f));
	REQUIRE(qmod(5.0, 3) == 2.0);
	REQUIRE(qmod(5.0, 3) == fmod(5.0, 3.0));
}
TEST_CASE("real/checksum") {
	REQUIRE(checksum_internal_table(0) == 0);
	REQUIRE(checksum_internal_table(2) == 3993919788);
	REQUIRE(checksum_internal_reflect(0, 8) == 0);
	REQUIRE(checksum_internal_reflect(2, 8) == 128);
	REQUIRE(checksum_internal_reflect(0, 32) == 0);
	REQUIRE(checksum_internal_reflect(2, 32) == 2147483648);

	std::vector<unsigned char> v1 = {65, 66, 67};
	unsigned int crc1 = get_checksum(v1);
	REQUIRE(crc1 == 1625830473);
	REQUIRE(verify_checksum(v1, crc1) == true);

	std::vector<unsigned char> v2 = {120, 121, 122};
	unsigned int crc2 = get_checksum(v2);
	REQUIRE(crc2 == 1178330758);
	REQUIRE(verify_checksum(v2, crc2) == true);
}

TEST_SUITE_END();

#endif // TESTS_UTIL_REAL
