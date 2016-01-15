/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

class RmTest: public BEE::Room {
	public:
		BEE::ViewData vw_main = {
			true,
			0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
			100, 100, 1800, 600,
			NULL,
			50, 50,
			0, 0
		};

		RmTest();
		int init();
};
RmTest::RmTest() : Room("RmTest", "test.hpp") {}

int RmTest::init() {
	// set room properties
	/*set_width(3000);
	set_height(500);

	set_view(0, &vw_main);
	set_is_views_enabled(true);*/

	// set backgrounds
	add_background(-1, bk_green, true, false, 0, 0, true, true, 10, 10, false);

	// set up instances
	add_instance(-1, obj_bee, 0, 0);

	return 0;
}
