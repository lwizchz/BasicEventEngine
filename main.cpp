/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
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

	// Declare some game options, see bee/game.hpp for the struct definition
	BEE::GameOptions options;
	//options.is_debug_enabled = true;

	try { // Catch all errors so that all resources will be freed properly
		// Initialize the game engine
		game = new BEE(argc, argv, BEE::get_standard_flags(), &rm_test, &options);
	} catch (std::string e) {
		std::cerr << e; // Output the caught exception string
		return 1; // Return 1 on exception during initialization
	}

	// Output initialization message
	game->messenger_send({"engine", "init"}, bee::E_MESSAGE::INFO,
		std::string("Initialized ") + MACRO_TO_STR(GAME_NAME) + " v" +
		std::to_string(GAME_VERSION_MAJOR) + "." + std::to_string(GAME_VERSION_MINOR) + "." + std::to_string(GAME_VERSION_RELEASE)
	);

	// Run the game engine event loop
	// This loop will run until the user closes the window, the game closes itself, or the game throws an exception
	int r = game->loop();

	// Clean up resources and quit SDL and other libraries
	game->close();

	delete game;
	game = nullptr;

	if (r != 0) { // If an exception was caught by the loop
		return 2; // Return 2 on exception during game loop
	}
	return 0; // Return 0 on successful run
}
