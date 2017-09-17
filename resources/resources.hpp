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
bee::Sprite* spr_none = nullptr;
bee::Sprite* spr_bee = nullptr;
bee::Sprite* spr_dot = nullptr;

// Define sounds
bee::Sound* snd_chirp = nullptr;

// Define backgrounds
bee::Background* bk_green = nullptr;

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

// Declare objects
bee::Object* obj_control = nullptr;
bee::Object* obj_bee = nullptr;

// Declare rooms
bee::Room* rm_test = nullptr;

// Include objects
#include "objects/control.hpp"
#include "objects/bee.hpp"

// Include rooms
#include "rooms/test.hpp"

/*
* bee::init_resources() - Initialize all game resources
* ! Note that loading is not required at this stage, just initialization
*/
int bee::init_resources() {
	try { // Catch any exceptions so that the engine can properly clean up
		// Init sprites
		spr_none = new Sprite("spr_none", "none.png");
			spr_none->load();
		spr_bee = new Sprite("spr_bee", "bee.png");
			spr_bee->set_subimage_amount(2, 100);
			spr_bee->set_speed(1.0);
		spr_dot = new Sprite("spr_dot", "dot.png");

		// Init sounds
		snd_chirp = new Sound("snd_chirp", "chirp.wav", false);

		// Init backgrounds
		bk_green = new Background("bk_green", "green.png");

		// Init fonts
		font_liberation = new Font("font_liberation", "liberation_mono.ttf", 24, false);

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

#define DEL(x) delete x; x=nullptr
/*
* bee::close_resources() - Destroy all game resources
*/
int bee::close_resources() {
	// Destroy sprites
	DEL(spr_bee);
	DEL(spr_dot);

	// Destroy sounds
	DEL(snd_chirp);

	// Destroy backgrounds
	DEL(bk_green);

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

	// Destroy objects
	DEL(obj_control);
	DEL(obj_bee);

	// Destroy rooms
	DEL(rm_test);

	is_initialized = false; // Unset the engine initialization flag

	return 0; // Return 0 on success
}
#undef DEL

#endif // RES_H
