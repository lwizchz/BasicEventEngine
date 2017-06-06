// Define sprites
bee::Sprite* spr_bee;
bee::Sprite* spr_dot;

// Define sounds
bee::Sound* snd_chirp;

// Define backgrounds
bee::Background* bk_green;

// Define fonts
bee::Font* font_liberation;

// Define paths
bee::Path* path_bee;

// Define timelines

// Define meshes
bee::Mesh* mesh_monkey;

// Define lights
bee::Light* lt_ambient;
bee::Light* lt_bee;

// Include and define objects
#include "sidp.hpp"

#include "objects/bee.hpp"
bee::Object* obj_bee;

// Include and define rooms
#include "rooms/test.hpp"
bee::Room* rm_test;

/*
* bee::init_resources() - Initialize all game resources
* ! Note that loading is note required at this stage, just initialization
*/
int bee::init_resources() {
	try { // Catch any exceptions so that the engine can properly clean up
		// Init sprites
		spr_bee = new Sprite("spr_bee", "bee.png");
			spr_bee->set_subimage_amount(2, 100);
			spr_bee->set_speed(1.0);
		spr_dot = new Sprite("spr_dot", "dot.png");

		// Init sounds
		snd_chirp = new Sound("snd_chirp", "chirp.wav", false);
			snd_chirp->load();

		// Init backgrounds
		bk_green = new Background("bk_green", "green.png");

		// Init fonts
		font_liberation = new Font("font_liberation", "liberation_mono.ttf", 24, false);
			font_liberation->load();

		// Init paths
		path_bee = new Path("path_bee", "");
			path_bee->add_coordinate(0.0, 0.0, 0.0, 1.0);
			path_bee->add_coordinate(200.0, 400.0, 0.0, 1.0);
			path_bee->add_coordinate(800.0, 400.0, 0.0, 1.0);
			path_bee->add_coordinate(500.0, 200.0, 0.0, 1.0);

		// Init timelines

		// Init meshes
		mesh_monkey = new Mesh("mesh_monkey", "monkey2.obj");
			mesh_monkey->load();

		// Init lights
		lt_ambient = new Light();
			//lt_ambient->set_color({255, 255, 255, 192});
			lt_ambient->set_color({255, 255, 255, 30});
		lt_bee = new Light();
			lt_bee->set_type(bee::E_LIGHT::POINT);
			//lt_bee->set_attenuation({2.0, 100.0, 2000.0, 0.0});
			lt_bee->set_attenuation({2.0, 100.0, 20000.0, 0.0});
			//lt_bee->set_attenuation({5.0, 1000.0, 40000.0, 0.0});
			lt_bee->set_color({255, 255, 255, 255});

		// Init objects
		obj_bee = new ObjBee();
			obj_bee->set_is_solid(true);

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
	DEL(obj_bee);

	// Destroy rooms
	DEL(rm_test);

	is_initialized = false; // Unset the engine initialization flag

	return 0; // Return 0 on success
}
