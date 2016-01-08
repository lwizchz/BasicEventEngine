/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#include <stdio.h>
#include <string>

#include "bee/game.hpp"
#include "resources/resources.hpp"

#ifdef _WINDOWS
#ifdef main
#undef main
#endif // main
#endif // _WINDOWS

int main(int argc, char* argv[]) {
	try {
		// Set game options;
		BEE::GameOptions options = {
			// Window flags
			true, false, true,
			true, true, false,
			true,
			// Renderer flags
			true,
			// Miscellaneous flags
			true,
			false
		};

		// Init game engine
		BEE* game = new BEE(argc, argv, &rm_test, &options);

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
