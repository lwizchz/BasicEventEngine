/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_RM_MENU
#define RES_RM_MENU 1

#include "../../bee/util.hpp"
#include "../../bee/all.hpp"

#include "../resources.hpp"

#include "rm_menu.hpp"

RmMenu::RmMenu() : Room("rm_menu", "rm_menu.cpp") {}

void RmMenu::init() {
	Room::init();

	set_width(1920);
	set_height(1080);

	get_phys_world()->set_gravity(btVector3(0.0, 0.0, 0.0));
	get_phys_world()->set_scale(100.0);

	//set_background_color({16, 16, 28, 255});
	add_background(bk_vortex, true, false, 0, 0, false, false, 0, 0, false);

	bee::set_window_title(
		std::string(MACRO_TO_STR(GAME_NAME)) + " - v" +
		std::to_string(GAME_VERSION_MAJOR) + "." + std::to_string(GAME_VERSION_MINOR) + "." + std::to_string(GAME_VERSION_RELEASE)
	);

	bee::render_set_3d(true);

	add_instance(-1, obj_menu, 0, 0, 0);
}

#endif // RES_RM_MENU
