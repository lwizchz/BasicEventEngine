/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
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

// Define Lights
bee::Light* lt_ambient = nullptr;
bee::Light* lt_bee = nullptr;

// Define Scripts
bee::Script* scr_test = nullptr;

// Declare Objects
bee::Object* obj_control = nullptr;
bee::Object* obj_bee = nullptr;

// Declare Rooms
bee::Room* rm_test = nullptr;

// Include Timelines
#include "timelines/tl_bee.hpp"

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
int bee::init_resources() {
	try { // Catch any exceptions so that the engine can properly clean up
		// Init Sprites
		spr_none = new Texture("spr_none", "none.png");
			spr_none->load();
		spr_bee = new Texture("spr_bee", "spr_bee.png");
			spr_bee->set_subimage_amount(2, 100);
		spr_dot = new Texture("spr_dot", "spr_dot.png");

		// Init Backgrounds
		bk_green = new Texture("bk_green", "bk_green.png");

		// Init Sounds
		snd_chirp = new Sound("snd_chirp", "snd_chirp.wav", false);

		// Init Fonts
		font_liberation = new Font("font_liberation", "liberation_mono.ttf", 24);

		// Init Paths
		path_bee = new Path("path_bee", "path_bee.json");
			path_bee->load();

		// Init Timelines
		tl_bee = new Timeline("tl_bee", "timelines/tl_bee.hpp");
			tl_bee_load();

		// Init Meshes
		mesh_monkey = new Mesh("mesh_monkey", "monkey2.obj");

		// Init Lights
		lt_ambient = new Light("lt_ambient", "");
			//lt_ambient->set_color({255, 255, 255, 192});
			lt_ambient->set_color({255, 255, 255, 30});
		lt_bee = new Light("lt_bee", "");
			lt_bee->set_type(bee::E_LIGHT::POINT);
			//lt_bee->set_attenuation({2.0, 100.0, 2000.0, 0.0});
			lt_bee->set_attenuation({2.0, 100.0, 20000.0, 0.0});
			//lt_bee->set_attenuation({5.0, 1000.0, 40000.0, 0.0});
			lt_bee->set_color({255, 255, 255, 255});

		// Init Scripts
		scr_test = new Script("scr_test", "scr_test.py");
			scr_test->load();

		// Init Objects
		obj_control = new ObjControl();
		obj_bee = new ObjBee();
			obj_bee->set_is_solid(true);
			obj_bee->set_sprite(spr_bee);

		// Init Rooms
		rm_test = new RmTest();

		is_initialized = true; // Set the engine initialization flag
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
int bee::close_resources() {
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

	// Destroy Lights
	DEL(lt_ambient);
	DEL(lt_bee);

	// Destroy Scripts
	DEL(scr_test);

	// Destroy Objects
	DEL(obj_control);
	DEL(obj_bee);

	// Destroy Rooms
	DEL(rm_test);

	is_initialized = false; // Unset the engine initialization flag

	return 0;
}
#undef DEL

#endif // RES
