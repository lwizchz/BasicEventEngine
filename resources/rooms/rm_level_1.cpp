/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_RM_LEVEL_1
#define RES_RM_LEVEL_1 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "rm_level_1.hpp"

RmLevel1::RmLevel1() : Room("rm_level_1", "rm_level_1.cpp") {}

void RmLevel1::init() {
	Room::init();

	set_width(1920);
	set_height(1080);

	get_phys_world()->set_gravity(btVector3(0.0, 0.0, 0.0));
	get_phys_world()->set_scale(100.0);

	set_background_color({16, 16, 28, 255});

	add_instance(-1, obj_lattice, 0, 0, 0);
	add_instance(-1, obj_player, 0, 0, 0);
}

#endif // RES_RM_LEVEL_1
