/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_RENDER_RGBA
#define TESTS_RENDER_RGBA 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/render/rgba.hpp"

TEST_SUITE_BEGIN("render");

TEST_CASE("rgba") {
	const int i = 2;
	for (int r=0; r<256; r+=i) {
		for (int g=0; g<256; g+=i) {
			for (int b=0; b<256; b+=i) {
				bee::RGBA c1 (r, g, b, 0);
				bee::RGBA c2;

				std::array<float,3> a = c1.get_hsv();
				c2.set_hsv(a);

				INFO("hsv: {" << a[0] << ", " << a[1] << ", " << a[2] << "}");

				REQUIRE(c1 == c2);
			}
		}
	}
}

TEST_SUITE_END();

#endif // TESTS_RENDER_RGBA
