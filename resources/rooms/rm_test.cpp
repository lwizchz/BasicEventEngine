/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_ROOM_TEST
#define RES_ROOM_TEST 1

#include "../resources.hpp"

#include "rm_test.hpp"

RmTest::RmTest() : Room("rm_test", "rm_test.hpp") {}
void RmTest::init() {
	Room::init();

	// Set room properties
	set_width(1920);
	set_height(1080);

	// Set up physics
	get_phys_world()->set_gravity(btVector3(0.0, 10.0, 0.0));
	get_phys_world()->set_scale(100.0);

	// Set backgrounds
	add_background(bk_green, true, false, 0, 0, true, true, 10, 10, false);

	// Set up instances
	if (get_instance_map().empty()) {
		set_instance_map("resources/rooms/rm_test.csv");
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

#endif // RES_ROOM_TEST
