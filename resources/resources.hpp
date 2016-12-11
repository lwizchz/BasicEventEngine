// Define sprites
BEE::Sprite* spr_bee;
BEE::Sprite* spr_dot;

// Define sounds
BEE::Sound* snd_chirp;

// Define backgrounds
BEE::Background* bk_green;

// Define fonts
BEE::Font* font_liberation;

// Define paths
BEE::Path* path_bee;

// Define meshes
BEE::Mesh* mesh_monkey;

// Define lights
BEE::Light* lt_ambient;
BEE::Light* lt_bee;

// Include and define objects
#include "objects/bee.hpp"
BEE::Object* obj_bee;

// Include and define rooms
#include "rooms/test.hpp"
BEE::Room* rm_test;

/*
* BEE::init_resources() - Initialize all game resources
* ! Note that loading is note required at this stage, just initialization
*/
int BEE::init_resources() {
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
			path_bee->add_coordinate(0,   0,   1.0);
			path_bee->add_coordinate(200, 400, 1.0);
			path_bee->add_coordinate(800, 400, 1.0);
			path_bee->add_coordinate(500, 200, 1.0);

		// Init meshes
		mesh_monkey = new Mesh("mesh_monkey", "monkey2.obj");
			mesh_monkey->load();

		// Init lights
		lt_ambient = new Light();
			//lt_ambient->set_color({255, 255, 255, 192});
			lt_ambient->set_color({255, 255, 255, 30});
		lt_bee = new Light();
			lt_bee->set_type(BEE_LIGHT_POINT);
			//lt_bee->set_attenuation({2.0, 100.0, 2000.0, 0.0});
			lt_bee->set_attenuation({2.0, 100.0, 20000.0, 0.0});
			//lt_bee->set_attenuation({5.0, 1000.0, 40000.0, 0.0});
			lt_bee->set_color({255, 255, 255, 255});

		// Init objects
		obj_bee = new ObjBee();

		// Init rooms
		rm_test = new RmTest();

		is_initialized = true; // Set the engine initialization flag
	} catch (...) {
		return 1; // Return 1 if any errors are encountered
	}

	return 0; // Return 0 on success
}
/*
* BEE::close_resources() - Destroy all game resources
*/
int BEE::close_resources() {
	// Destroy sprites
	delete spr_bee;
	delete spr_dot;

	// Destroy sounds
	delete snd_chirp;

	// Destroy backgrounds
	delete bk_green;

	// Destroy fonts
	delete font_liberation;

	// Destroy paths
	delete path_bee;

	// Destroy meshes
	delete mesh_monkey;

	// Destroy lights
	delete lt_ambient;
	delete lt_bee;

	// Destroy objects
	delete obj_bee;

	// Destroy rooms
	delete rm_test;

	is_initialized = false; // Unset the engine initialization flag

	return 0; // Return 0 on success
}
