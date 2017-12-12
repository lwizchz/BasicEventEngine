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
extern bee::Texture* spr_none;
extern bee::Texture* spr_bee;
extern bee::Texture* spr_dot;

// Define backgrounds
extern bee::Texture* bk_green;

// Define sounds
extern bee::Sound* snd_chirp;

// Define fonts
extern bee::Font* font_liberation;

// Define paths
extern bee::Path* path_bee;

// Define timelines

// Define meshes
extern bee::Mesh* mesh_monkey;

// Define lights
extern bee::Light* lt_ambient;
extern bee::Light* lt_bee;

// Declare objects
extern bee::Object* obj_control;
extern bee::Object* obj_bee;

// Declare rooms
extern bee::Room* rm_test;

namespace bee {
	int init_resources();
	int close_resources();
}

#endif // RES_H
