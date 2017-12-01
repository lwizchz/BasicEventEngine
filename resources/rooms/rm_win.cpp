/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_RM_WIN
#define RES_RM_WIN 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "rm_win.hpp"

RmWin::RmWin() : Room("rm_win", "rm_win.cpp") {}

void RmWin::init() {
	Room::init();

	set_width(1920);
	set_height(1080);

	get_phys_world()->set_gravity(btVector3(0.0, 0.0, 0.0));
	get_phys_world()->set_scale(100.0);

	set_background_color({16, 16, 28, 255});
	//add_background(bk_vortex, true, false, 0, 0, false, false, 0, 0, false);

	add_instance(-1, obj_lattice, 0, 0, 0);
	add_instance(-1, obj_player, 0, 0, 0);

	for (size_t i=0; i<20; ++i) {
		add_instance(-1, obj_enemy, 0, 0, 0);
	}
	for (size_t i=0; i<2; ++i) {
		add_instance(-1, obj_bee, 0, 0, 0);
	}
}

#endif // RES_RM_WIN
