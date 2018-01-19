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

TEST_CASE("real/math") {
	REQUIRE(util::sign(5) == 1);
	REQUIRE(util::sign(0) == 0);
	REQUIRE(util::sign(-5) == -1);
	REQUIRE(util::sign(5.0) == 1);
	REQUIRE(util::sign(0.0) == 0);
	REQUIRE(util::sign(-5.0) == -1);
	REQUIRE(util::sqr(5) == 25);
	REQUIRE(util::sqr(5.0) == 25.0);
	REQUIRE(util::logn(5.0, 1.0) == 0.0);
	REQUIRE(util::logn(5.0, 5.0) == doctest::Approx(1.0));
	REQUIRE(util::logn(5.0, 10.0) == doctest::Approx(1.431).epsilon(0.001));
	REQUIRE(util::degtorad(90.0) == doctest::Approx(PI/2.0));
	REQUIRE(util::degtorad(360.0) == doctest::Approx(2.0*PI));
	REQUIRE(util::radtodeg(PI) == doctest::Approx(180.0));
	REQUIRE(util::radtodeg(PI/3.0) == doctest::Approx(60.0));
	REQUIRE(util::opposite_angle(0.0) == 180.0);
	REQUIRE(util::opposite_angle(60.0) == 240.0);
	REQUIRE(util::opposite_angle(270.0) == 90.0);
	REQUIRE(util::opposite_angle(360.0) == 180.0);
	REQUIRE(util::opposite_angle(-90.0) == 90.0);
	REQUIRE(util::absolute_angle(0.0) == 0.0);
	REQUIRE(util::absolute_angle(360.0) == 0.0);
	REQUIRE(util::absolute_angle(-90.0) == 270.0);
	REQUIRE(util::absolute_angle(810.0) == 90.0);
	REQUIRE(util::absolute_angle(-810.0) == 270.0);
}
TEST_CASE("real/movement") {
	REQUIRE(util::direction_of(1.0, 2.0, 3.0, 4.0) == doctest::Approx(util::opposite_angle(util::direction_of(3.0, 4.0, 1.0, 2.0))));
	REQUIRE(util::direction_of(0.0, 0.0, 1.0, 0.0) == doctest::Approx(0.0));
	REQUIRE(util::direction_of(0.0, 0.0, 0.0, 1.0) == doctest::Approx(90.0));
	REQUIRE(util::direction_of(0.0, 0.0, -1.0, 0.0) == doctest::Approx(180.0));
	REQUIRE(util::direction_of(0.0, 0.0, 0.0, -1.0) == doctest::Approx(270.0));
	REQUIRE(util::distance(0.0, 0.0, 1.0, 0.0) == 1.0);
	REQUIRE(util::distance(0.0, 0.0, 3.0, 4.0) == 5.0);
	REQUIRE(util::distance(0.0, 0.0, 0.0, 0.0) == 0.0);

	REQUIRE(util::bt_to_glm3(btVector3(0.0, 1.0, 2.0)) == glm::vec3(0.0, 1.0, 2.0));
	REQUIRE(util::glm_to_bt3(glm::vec3(0.0, 1.0, 2.0)) == btVector3(0.0, 1.0, 2.0));
	REQUIRE(util::bt_to_glm3(util::glm_to_bt3(glm::vec3(0.0, 1.0, 2.0))) == glm::vec3(0.0, 1.0, 2.0));
}
TEST_CASE("real/bounds") {
	REQUIRE(util::is_between(5, 3, 6) == true);
	REQUIRE(util::is_between(6, 3, 6) == true);
	REQUIRE(util::is_between(7, 3, 6) == false);
	REQUIRE(util::is_between(5, 6, 3) == false);
	REQUIRE(util::is_between(5.0, 3.0, 6.0) == true);
	REQUIRE(util::is_angle_between(90, 0, 180) == true);
	REQUIRE(util::is_angle_between(180, 0, 180) == true);
	REQUIRE(util::is_angle_between(270, 0, 180) == false);
	REQUIRE(util::is_angle_between(90.0, 0.0, 180.0) == true);
	REQUIRE(util::is_angle_between(0, 270, 90) == true);
	REQUIRE(util::is_angle_between(360, 270, 90) == true);
	REQUIRE(util::fit_bounds(5, 3, 6) == 5);
	REQUIRE(util::fit_bounds(6, 3, 6) == 6);
	REQUIRE(util::fit_bounds(7, 3, 6) == 6);
	REQUIRE(util::fit_bounds(10, 6, 3) == 10);
	REQUIRE(util::fit_bounds(5, 6, 3) == 6);
	REQUIRE(util::fit_bounds(5.0, 3.0, 6.0) == 5.0);
	REQUIRE(util::qmod(5, 3) == 2);
	REQUIRE(util::qmod(3, 3) == 0);
	REQUIRE(util::qmod(2, 3) == 2);
	REQUIRE(util::qmod(5.f, 3) == 2.f);
	REQUIRE(util::qmod(5.f, 3) == fmod(5.f, 3.f));
	REQUIRE(util::qmod(5.0, 3) == 2.0);
	REQUIRE(util::qmod(5.0, 3) == fmod(5.0, 3.0));
}
TEST_CASE("real/random") {
	REQUIRE(util::random::get(20) < 20u);
	REQUIRE(util::is_between(static_cast<int>(util::random::get_range(20, 40)), 20, 40));
	REQUIRE(util::random::set_seed(5) == 5u);
	REQUIRE(util::random::get(0) == 953453411u);
	REQUIRE(util::random::reset_seed() != 1u);
	REQUIRE(util::random::randomize() != 1u);
}
TEST_CASE("real/checksum") {
	REQUIRE(util::checksum::internal::reflect(0, 8) == 0);
	REQUIRE(util::checksum::internal::reflect(2, 8) == 128);
	REQUIRE(util::checksum::internal::reflect(0, 32) == 0);
	REQUIRE(util::checksum::internal::reflect(2, 32) == 2147483648);

	REQUIRE(util::checksum::internal::table(0) == 0);
	REQUIRE(util::checksum::internal::table(2) == 3993919788);

	std::vector<unsigned char> v1 = {65, 66, 67};
	unsigned int crc1 = util::checksum::get(v1);
	REQUIRE(crc1 == 1625830473);
	REQUIRE(util::checksum::verify(v1, crc1) == true);

	std::vector<unsigned char> v2 = {120, 121, 122};
	unsigned int crc2 = util::checksum::get(v2);
	REQUIRE(crc2 == 1178330758);
	REQUIRE(util::checksum::verify(v2, crc2) == true);
}

TEST_SUITE_END();

#endif // TESTS_UTIL_REAL
