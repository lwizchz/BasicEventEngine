/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of EGE.
* EGE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#include <stdio.h>

#include "ege/game.hpp"

int main(int argc, char* argv[]) {
	RmTest rm_test;

	try {
		// Init game engine
		EGE* game = new EGE(argc, argv, &rm_test);

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
