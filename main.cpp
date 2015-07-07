#include <stdio.h>

#include "ege/game.hpp"

int main(int argc, char* argv[]) {
	// Init game engine
	game.g = new EGE(argc, argv, NULL);
	
	// Set first room
	RmTest rm_test;
	if (game.g->change_room(&rm_test)) {
		return 1;
	}
	
	// Loop event handling
	game.g->loop();
	
	// Clean up
	game.g->close();
	
	return 0;
}
