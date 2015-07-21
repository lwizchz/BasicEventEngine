bool is_initialized = false;

Sprite* spr_bee;

Sound* snd_chirp;

Background* bk_green;

Font* font_liberation;

#include "objects/bee.hpp"
ObjBee* obj_bee;

#include "rooms/test.hpp"
//RmTest* rm_test;

int init_resources() {
	try {
		// Init sprites
		spr_bee = new Sprite("spr_bee", "bee.png");
			spr_bee->set_subimage_amount(2, 100);
			spr_bee->set_speed(0.05);

		// Init sounds
		snd_chirp = new Sound("snd_chirp", "chirp.wav", false);
			snd_chirp->load();

		// Init backgrounds
		bk_green = new Background("bk_green", "green.png");

		// Init fonts
		font_liberation = new Font("font_liberation", "liberation_mono.ttf", 24);
			font_liberation->load();

		// Init paths

		// Init objects
		obj_bee = new ObjBee();

		// Init rooms
		//rm_test = new RmTest();

		is_initialized = true;
	} catch (...) {
		return 1;
	}

	return 0;
}
int close_resources() {
	delete spr_bee;
	delete snd_chirp;
	delete bk_green;
	delete font_liberation;
	delete obj_bee;
	//delete rm_test;

	is_initialized = false;

	return 0;
}
