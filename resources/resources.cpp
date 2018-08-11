/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef RES
#define RES 1

#include "resources.hpp"

// Define Sprites
bee::Texture* spr_none = nullptr;
bee::Texture* spr_bee = nullptr;
bee::Texture* spr_dot = nullptr;

// Define Backgrounds
bee::Texture* bk_green = nullptr;

// Define Sounds
bee::Sound* snd_chirp = nullptr;

// Define Fonts
bee::Font* font_liberation = nullptr;

// Define Paths
bee::Path* path_bee = nullptr;

// Define Timelines
bee::Timeline* tl_bee = nullptr;

// Define Meshes
bee::Mesh* mesh_monkey = nullptr;
bee::Mesh* mesh_spider = nullptr;

// Define Lights
bee::Light* lt_ambient = nullptr;
bee::Light* lt_bee = nullptr;

// Define Scripts
bee::Script* scr_test = nullptr;

// Declare Objects
bee::Object* obj_control = nullptr;
bee::Object* obj_bee = nullptr;
bee::Object* obj_mesh = nullptr;

// Declare Rooms
bee::Room* rm_test = nullptr;

// Include Objects
#include "objects/obj_control.hpp"
#include "objects/obj_bee.hpp"

// Include Rooms
#include "rooms/rm_test.hpp"

/**
* Initialize all game resources.
* @note Loading is not required at this stage, just initialization.
*
* @retval 0 success
* @retval 1 failed
*/
int init_resources() {
	try { // Catch any exceptions so that the engine can properly clean up
		// Init Sprites
		spr_none = new bee::Texture("spr_none", "none.png");
			spr_none->load();
		spr_bee = new bee::Texture("spr_bee", "spr_bee.png");
			spr_bee->set_subimage_amount(2, 100);
		spr_dot = new bee::Texture("spr_dot", "spr_dot.png");

		// Init Backgrounds
		bk_green = new bee::Texture("bk_green", "bk_green.png");

		// Init Sounds
		snd_chirp = new bee::Sound("snd_chirp", "snd_chirp.wav", false);

		// Init Fonts
		font_liberation = new bee::Font("font_liberation", "liberation_mono.ttf", 24);

		// Init Paths
		path_bee = new bee::Path("path_bee", "path_bee.json");

		// Init Timelines
		tl_bee = new bee::Timeline("tl_bee", "tl_bee.py");

		// Init Meshes
		mesh_monkey = new bee::Mesh("mesh_monkey", "monkey2.obj");
		mesh_spider = new bee::Mesh("mesh_spider", "spider.fbx");

		// Init Lights
		lt_ambient = new bee::Light("lt_ambient", "lt_ambient.json");
		lt_bee = new bee::Light("lt_bee", "lt_bee.json");

		// Init Scripts
		scr_test = new bee::Script("scr_test", "scr_test.py");

		// Init Objects
		obj_control = new ObjControl();
		obj_bee = new ObjBee();
			obj_bee->set_sprite(spr_bee);
		obj_mesh = new bee::ObjScript("obj_mesh.py");

		// Init Rooms
		rm_test = new RmTest();

		bee::is_initialized = true; // Set the engine initialization flag
	} catch (...) {
		return 1;
	}

	return 0;
}

#define DEL(x) if (x!=nullptr) {delete x; x=nullptr;}
/**
* Destroy all game resources.
*
* @retval 0 success
*/
int close_resources() {
	// Destroy Sprites
	DEL(spr_bee);
	DEL(spr_dot);

	// Destroy Backgrounds
	DEL(bk_green);

	// Destroy Sounds
	DEL(snd_chirp);

	// Destroy Fonts
	DEL(font_liberation);

	// Destroy Paths
	DEL(path_bee);

	// Destroy Timelines
	DEL(tl_bee);

	// Destroy Meshes
	DEL(mesh_monkey);
	DEL(mesh_spider);

	// Destroy Lights
	DEL(lt_ambient);
	DEL(lt_bee);

	// Destroy Scripts
	DEL(scr_test);

	// Destroy Objects
	DEL(obj_control);
	DEL(obj_bee);
	DEL(obj_mesh);

	// Destroy Rooms
	DEL(rm_test);

	bee::is_initialized = false; // Unset the engine initialization flag

	return 0;
}
#undef DEL

#endif // RES
