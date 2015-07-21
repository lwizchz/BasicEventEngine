/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#include <stdio.h>

#include "bee/game.hpp"

int main(int argc, char* argv[]) {
	RmTest rm_test;

	try {
		// Init game engine
		BEE* game = new BEE(argc, argv, &rm_test);

		// Loop event handling
		game->loop();

		// Clean up
		game->close();
	} catch (std::string e) {
		std::cerr << e;
		return 1;
	}

	return 0;
}
