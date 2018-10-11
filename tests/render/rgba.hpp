/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef TESTS_RENDER_RGBA
#define TESTS_RENDER_RGBA 1

#include "doctest.h" // Include the required unit testing library

#include "../../bee/render/rgba.hpp"

#include "../../bee/util/dates.hpp"

TEST_SUITE_BEGIN("render");

TEST_CASE("rgba") {
	const int i = 4; // Increment by i to avoid spending time testing every single case
	const int start = util::date::current_datetime() % i;
	INFO("start: " << start << ", increment: " << i);

	for (int r=start; r<256; r+=i) {
		for (int g=start; g<256; g+=i) {
			for (int b=start; b<256; b+=i) {
				bee::RGBA c1 (r, g, b, 0);
				bee::RGBA c2;

				std::array<float,3> a = c1.get_hsv();
				c2.set_hsv(a);

				INFO("[c1(" << r << ", " << g << ", " << b << "),");
				int c2r = c2.r, c2g = c2.g, c2b = c2.b;
				INFO(" c2(" << c2r << ", " << c2g << ", " << c2b << ")]");
				REQUIRE(c1 == c2);
			}
		}
	}
}

TEST_SUITE_END();

#endif // TESTS_RENDER_RGBA
