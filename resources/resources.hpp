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

#include "extras.hpp"

// Define sprites
extern bee::Texture* spr_none;

extern bee::Texture* spr_logo;

extern VectorSprite* vs_enemy;
extern VectorSprite* vs_bee;

// Define backgrounds
extern bee::Texture* bk_vortex;

// Define sounds
extern bee::Sound* snd_music_main;

extern bee::Sound* snd_shoot;
extern bee::Sound* snd_super;
extern bee::Sound* snd_hit;
extern bee::Sound* snd_gameover;

// Define fonts

// Define paths

// Define timelines

// Define meshes

// Define lights

// Define objects
extern bee::Object* obj_menu;

extern bee::Object* obj_lattice;
extern bee::Object* obj_player;
extern bee::Object* obj_enemy;
extern bee::Object* obj_bee;

// Define rooms
extern bee::Room* rm_menu;
extern bee::Room* rm_levels;
extern bee::Room* rm_win;

namespace bee {
	int init_resources();
	int close_resources();
}

#endif // RES_H
