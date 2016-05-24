BEE::Sprite* spr_bee;
BEE::Sprite* spr_dot;

BEE::Sound* snd_chirp;
BEE::Sound* snd_music;

BEE::Background* bk_green;

BEE::Font* font_liberation;

BEE::Path* path_bee;

#include "objects/bee.hpp"
ObjBee* obj_bee;

#include "rooms/test.hpp"
BEE::Room* rm_test;

int BEE::init_resources() {
	try {
		// Init sprites
		spr_bee = new Sprite("spr_bee", "bee.png");
			spr_bee->set_subimage_amount(2, 100);
			spr_bee->set_speed(0.5);
		spr_dot = new Sprite("spr_dot", "dot.png");

		// Init sounds
		snd_chirp = new Sound("snd_chirp", "chirp.wav", false);
			snd_chirp->load();
		snd_music = new Sound("snd_chirp", "music2.wav", false);
			snd_music->load();

		// Init backgrounds
		bk_green = new Background("bk_green", "green.png");

		// Init fonts
		font_liberation = new Font("font_liberation", "liberation_mono.ttf", 24, false);
			font_liberation->load();

		// Init paths
		path_bee = new Path("path_bee", "");
			path_bee->add_coordinate(0, 0, 1.0);
			path_bee->add_coordinate(200, 400, 1.0);
			path_bee->add_coordinate(800, 400, 1.0);
			path_bee->add_coordinate(500, 200, 1.0);

		// Init objects
		obj_bee = new ObjBee();

		// Init rooms
		rm_test = new RmTest();

		is_initialized = true;
	} catch (...) {
		return 1;
	}

	return 0;
}
int BEE::close_resources() {
	delete spr_bee;
	delete spr_dot;
	delete snd_chirp;
	delete snd_music;
	delete bk_green;
	delete font_liberation;
	delete path_bee;
	delete obj_bee;
	delete rm_test;

	is_initialized = false;

	return 0;
}
