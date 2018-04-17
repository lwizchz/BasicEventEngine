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

// Define sprites
bee::Texture* spr_none = nullptr;
bee::Texture* spr_bee = nullptr;
bee::Texture* spr_dot = nullptr;

// Define backgrounds
bee::Texture* bk_green = nullptr;

// Define sounds
bee::Sound* snd_chirp = nullptr;

// Define fonts
bee::Font* font_liberation = nullptr;

// Define paths
bee::Path* path_bee = nullptr;

// Define timelines

// Define meshes
bee::Mesh* mesh_monkey = nullptr;

// Define lights
bee::Light* lt_ambient = nullptr;
bee::Light* lt_bee = nullptr;

// Define scripts
bee::Script* scr_test = nullptr;

// Declare objects
bee::Object* obj_control = nullptr;
bee::Object* obj_bee = nullptr;

// Declare rooms
bee::Room* rm_test = nullptr;

// Include objects
#include "objects/obj_control.hpp"
#include "objects/obj_bee.hpp"

// Include rooms
#include "rooms/rm_test.hpp"

/*
* bee::init_resources() - Initialize all game resources
* ! Note that loading is not required at this stage, just initialization
*/
int bee::init_resources() {
	try { // Catch any exceptions so that the engine can properly clean up
		// Init sprites
		spr_none = new Texture("spr_none", "none.png");
			spr_none->load();
		spr_bee = new Texture("spr_bee", "spr_bee.png");
			spr_bee->set_subimage_amount(2, 100);
		spr_dot = new Texture("spr_dot", "spr_dot.png");

		// Init backgrounds
		bk_green = new Texture("bk_green", "bk_green.png");

		// Init sounds
		snd_chirp = new Sound("snd_chirp", "snd_chirp.wav", false);

		// Init fonts
		font_liberation = new Font("font_liberation", "liberation_mono.ttf", 24);

		// Init paths
		path_bee = new Path("path_bee", "");
			path_bee->add_coordinate(0.0, 0.0, 0.0, 1.0);
			path_bee->add_coordinate(200.0, 400.0, 0.0, 1.0);
			path_bee->add_coordinate(800.0, 400.0, 0.0, 1.0);
			path_bee->add_coordinate(500.0, 200.0, 0.0, 1.0);

		// Init timelines

		// Init meshes
		mesh_monkey = new Mesh("mesh_monkey", "monkey2.obj");

		// Init lights
		lt_ambient = new Light("lt_ambient", "");
			//lt_ambient->set_color({255, 255, 255, 192});
			lt_ambient->set_color({255, 255, 255, 30});
		lt_bee = new Light("lt_bee", "");
			lt_bee->set_type(bee::E_LIGHT::POINT);
			//lt_bee->set_attenuation({2.0, 100.0, 2000.0, 0.0});
			lt_bee->set_attenuation({2.0, 100.0, 20000.0, 0.0});
			//lt_bee->set_attenuation({5.0, 1000.0, 40000.0, 0.0});
			lt_bee->set_color({255, 255, 255, 255});

		// Init scripts
		scr_test = new Script("scr_test", "scr_test.py");
			scr_test->load();

		// Init objects
		obj_control = new ObjControl();
		obj_bee = new ObjBee();
			obj_bee->set_is_solid(true);
			obj_bee->set_sprite(spr_bee);

		// Init rooms
		rm_test = new RmTest();

		is_initialized = true; // Set the engine initialization flag
	} catch (...) {
		return 1; // Return 1 if any errors are encountered
	}

	return 0; // Return 0 on success
}

#define DEL(x) if (x!=nullptr) {delete x; x=nullptr;}
/*
* bee::close_resources() - Destroy all game resources
*/
int bee::close_resources() {
	// Destroy sprites
	DEL(spr_bee);
	DEL(spr_dot);

	// Destroy backgrounds
	DEL(bk_green);

	// Destroy sounds
	DEL(snd_chirp);

	// Destroy fonts
	DEL(font_liberation);

	// Destroy paths
	DEL(path_bee);

	// Destroy timelines

	// Destroy meshes
	DEL(mesh_monkey);

	// Destroy lights
	DEL(lt_ambient);
	DEL(lt_bee);

	// Destroy scripts
	DEL(scr_test);

	// Destroy objects
	DEL(obj_control);
	DEL(obj_bee);

	// Destroy rooms
	DEL(rm_test);

	is_initialized = false; // Unset the engine initialization flag

	return 0; // Return 0 on success
}
#undef DEL

#endif // RES
