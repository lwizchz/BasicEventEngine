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
bee::Sprite* spr_none = nullptr;

bee::Sprite* spr_logo = nullptr;

// Define sounds

// Define backgrounds
bee::Background* bk_vortex = nullptr;

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

bee::Room* rm_level_1 = nullptr;

// Include objects
#include "objects/obj_menu.hpp"

#include "objects/obj_lattice.hpp"
#include "objects/obj_player.hpp"
#include "objects/obj_enemy.hpp"
#include "objects/obj_bee.hpp"

// Include rooms
#include "rooms/rm_menu.hpp"

#include "rooms/rm_level_1.hpp"

/*
* bee::init_resources() - Initialize all game resources
* ! Note that loading is not required at this stage, just initialization
*/
int bee::init_resources() {
	try { // Catch any exceptions so that the engine can properly clean up
		// Init sprites
		spr_none = new Sprite("spr_none", "none.png");
			spr_none->load();

		spr_logo = new Sprite("spr_logo", "menu/logo.png");
			spr_logo->load();

		// Init sounds

		// Init backgrounds
		bk_vortex = new Background("bk_vortex", "bk_vortex.png");
			bk_vortex->load();

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

		rm_level_1 = new RmLevel1();

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

	// Destroy sounds

	// Destroy backgrounds
	DEL(bk_vortex);

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

	DEL(rm_level_1);

	is_initialized = false; // Unset the engine initialization flag

	return 0; // Return 0 on success
}
#undef DEL

#endif // RES
