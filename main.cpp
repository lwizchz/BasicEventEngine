#include <stdio.h>

#include "ege/game.hpp"

int main(int argc, char* argv[]) {
	RmTest rm_test;

	try {
		// Init game engine
		EGE* game = new EGE(argc, argv, &rm_test);

		// Loop event handling
		if (game->loop()) {
			return 2;
		}

		// Clean up
		game->close();
	} catch (std::string e) {
		std::cerr << e;
		return 1;
	}

	return 0;
}
