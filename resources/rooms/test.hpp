/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

class RmTest: public bee::Room {
	public:
		bee::ViewData vw_main = {
			true,
			0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
			100, 100, 1800, 600,
			nullptr,
			50, 50,
			0, 0
		};

		RmTest();
		int init() override final;
};
RmTest::RmTest() : Room("RmTest", "test.hpp") {}

int RmTest::init() {
	// set room properties
	/*set_width(3000);
	set_height(500);

	set_view(0, &vw_main);
	set_is_views_enabled(true);*/

	set_width(1920);
	set_height(1080);

	// Set up physics
	get_phys_world()->set_gravity(btVector3(0.0, 10.0, 0.0));
	get_phys_world()->set_scale(100.0);

	// set backgrounds
	add_background(bk_green, true, false, 0, 0, true, true, 10, 10, false);

	// set up instances
	if (get_instance_map().empty()) {
		set_instance_map("resources/rooms/test.csv");
	}
	load_instance_map();

	return 0;
}
