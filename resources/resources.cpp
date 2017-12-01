/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef RES
#define RES 1

#include "resources.hpp"

// Define sprites
bee::Texture* spr_none = nullptr;

bee::Texture* spr_logo = nullptr;

VectorSprite* vs_enemy = nullptr;
VectorSprite* vs_bee = nullptr;

// Define backgrounds
bee::Texture* bk_vortex = nullptr;

// Define sounds
bee::Sound* snd_music_main = nullptr;

bee::Sound* snd_shoot = nullptr;
bee::Sound* snd_super = nullptr;
bee::Sound* snd_hit = nullptr;
bee::Sound* snd_gameover = nullptr;

// Define fonts

// Define paths

// Define timelines

// Define meshes

// Define lights

// Define objects
bee::Object* obj_menu = nullptr;

bee::Object* obj_lattice = nullptr;
bee::Object* obj_player = nullptr;
bee::Object* obj_enemy = nullptr;
bee::Object* obj_bee = nullptr;

// Define rooms
bee::Room* rm_menu = nullptr;

bee::Room* rm_levels = nullptr;
bee::Room* rm_win = nullptr;

// Include objects
#include "objects/obj_menu.hpp"

#include "objects/obj_lattice.hpp"
#include "objects/obj_player.hpp"
#include "objects/obj_enemy.hpp"
#include "objects/obj_bee.hpp"

// Include rooms
#include "rooms/rm_menu.hpp"
#include "rooms/rm_levels.hpp"
#include "rooms/rm_win.hpp"

/*
* bee::init_resources() - Initialize all game resources
* ! Note that loading is not required at this stage, just initialization
*/
int bee::init_resources() {
	try { // Catch any exceptions so that the engine can properly clean up
		// Init sprites
		spr_none = new Texture("spr_none", "none.png");
			spr_none->load();

		spr_logo = new Texture("spr_logo", "menu/logo.png");
			spr_logo->load();

		vs_enemy = new VectorSprite("resources/sprites/enemy.csv");
		vs_bee = new VectorSprite("resources/sprites/bee.csv");

		// Init backgrounds
		bk_vortex = new Texture("bk_vortex", "bk_vortex.png");
			bk_vortex->load();

		// Init sounds
		snd_music_main = new Sound("snd_music_main", "snd_music_main.wav", false);
			snd_music_main->load();

		snd_shoot = new Sound("snd_shoot", "snd_shoot.wav", false);
			snd_shoot->load();
		snd_super = new Sound("snd_super", "snd_super.wav", false);
			snd_super->load();
		snd_hit = new Sound("snd_hit", "snd_hit.wav", false);
			snd_hit->load();
		snd_gameover = new Sound("snd_gameover", "snd_gameover.wav", false);
			snd_gameover->load();

		// Init fonts

		// Init paths

		// Init timelines

		// Init meshes

		// Init lights

		// Init objects
		obj_menu = new ObjMenu();

		obj_lattice = new ObjLattice();
		obj_player = new ObjPlayer();
		obj_enemy = new ObjEnemy();
		obj_bee = new ObjBee();

		// Init rooms
		rm_menu = new RmMenu();
		rm_levels = new RmLevels();
		rm_win = new RmWin();

		is_initialized = true; // Set the engine initialization flag
	} catch (...) {
		return 1; // Return 1 if any errors are encountered
	}

	return 0; // Return 0 on success
}

#define DEL(x) delete x; x=nullptr
/*
* bee::close_resources() - Destroy all game resources
*/
int bee::close_resources() {
	// Destroy sprites
	DEL(spr_none);

	DEL(spr_logo);

	DEL(vs_enemy);
	DEL(vs_bee);

	// Destroy backgrounds
	DEL(bk_vortex);

	// Destroy sounds
	DEL(snd_music_main);

	DEL(snd_shoot);
	DEL(snd_super);
	DEL(snd_hit);
	DEL(snd_gameover);

	// Destroy fonts

	// Destroy paths

	// Destroy timelines

	// Destroy meshes

	// Destroy lights

	// Destroy objects
	DEL(obj_menu);

	DEL(obj_lattice);
	DEL(obj_player);
	DEL(obj_enemy);
	DEL(obj_bee);

	// Destroy rooms
	DEL(rm_menu);
	DEL(rm_levels);
	DEL(rm_win);

	is_initialized = false; // Unset the engine initialization flag

	return 0; // Return 0 on success
}
#undef DEL

#endif // RES
