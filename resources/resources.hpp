/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES_H
#define RES_H 1

#include "../bee/util.hpp"
#include "../bee/all.hpp"

// Define sprites
extern bee::Sprite* spr_none;

extern bee::Sprite* spr_logo;

// Define sounds

// Define backgrounds

// Define fonts

// Define paths

// Define timelines

// Define meshes

// Define lights

// Define objects
extern bee::Object* obj_menu;

extern bee::Object* obj_player;

// Define rooms
extern bee::Room* rm_menu;

extern bee::Room* rm_level_1;

namespace bee {
	int init_resources();
	int close_resources();
}

#endif // RES_H
