/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#include "bee/game.hpp" // Include the engine headers
#include "resources/resources.hpp" // Include the resource definitions

// Undefine main() if it has been previously defined on the Windows platform
// I think this occurs because of SDL2/SDL_main.hpp
#ifdef _WIN32
#ifdef main
#undef main
#endif // main
#endif // _WIN32

int main(int argc, char* argv[]) {
	BEE* game = nullptr;

	// Declare some game options, see bee/game.hpp for the type definition
	BEE::GameOptions options = {
		// Window flags
		true,  true,
		true,  true,
		false, true,
		// Renderer flags
		BEE_RENDERER_OPENGL4,
		false, false,
		// Miscellaneous flags
		true,
		false
	};

	try { // Catch all errors so that all resources will be freed properly
		// Initialize the game engine
		game = new BEE(argc, argv, BEE::get_standard_flags(), &rm_test, &options);
	} catch (std::string e) {
		std::cerr << e; // Output the caught exception string
		return 1; // Return 1 on caught exception
	}

	// Run the game engine event loop
	// This loop will run until the user closes the window, the game closes itself, or the game throws an exception
	game->loop();

	// Clean up resources and quit SDL and other libraries
	game->close();

	return 0; // Return 0 on successful run
}
