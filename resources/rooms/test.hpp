/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_ROOM_TEST_H
#define RES_ROOM_TEST_H 1

class RmTest: public bee::Room {
	public:
		bee::ViewData vw_main = bee::ViewData(
			true,
			{0, 0, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT},
			{100, 100, 1800, 600},
			nullptr,
			50, 50,
			0, 0
		);

		RmTest();
		void init();
};
RmTest::RmTest() : Room("RmTest", "test.hpp") {}
void RmTest::init() {
	Room::init();

	// Set room properties
	/*set_width(3000);
	set_height(500);

	set_view(0, &vw_main);
	set_is_views_enabled(true);*/

	set_width(1920);
	set_height(1080);

	// Set up physics
	get_phys_world()->set_gravity(btVector3(0.0, 10.0, 0.0));
	get_phys_world()->set_scale(100.0);

	// Set backgrounds
	add_background(bk_green, true, false, 0, 0, true, true, 10, 10, false);

	// Set up instances
	if (get_instance_map().empty()) {
		set_instance_map("resources/rooms/test.csv");
	}
	load_instance_map();

	// Load required resources for this room
	if (spr_bee->get_is_loaded()) {
		return;
	}

	bee::loader::queue(spr_bee);
	bee::loader::queue(snd_chirp);
	bee::loader::queue(bk_green);
	bee::loader::queue(font_liberation);
	bee::loader::queue(mesh_monkey);

	bee::loader::load();
	bee::loader::clear();
}

#endif // RES_ROOM_TEST_H
