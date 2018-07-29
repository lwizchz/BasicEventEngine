/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef RES_H
#define RES_H 1

#include "../bee/util.hpp"
#include "../bee/all.hpp"

// Define Sprites
extern bee::Texture* spr_none;
extern bee::Texture* spr_bee;
extern bee::Texture* spr_dot;

// Define Backgrounds
extern bee::Texture* bk_green;

// Define Sounds
extern bee::Sound* snd_chirp;

// Define Fonts
extern bee::Font* font_liberation;

// Define Paths
extern bee::Path* path_bee;

// Define Timelines
extern bee::Timeline* tl_bee;

// Define Meshes
extern bee::Mesh* mesh_monkey;
extern bee::Mesh* mesh_spider;

// Define Lights
extern bee::Light* lt_ambient;
extern bee::Light* lt_bee;

// Define Scripts
extern bee::Script* scr_test;

// Declare Objects
extern bee::Object* obj_control;
extern bee::Object* obj_bee;
extern bee::Object* obj_mesh;

// Declare Rooms
extern bee::Room* rm_test;

namespace bee {
	int init_resources();
	int close_resources();
}

#endif // RES_H
