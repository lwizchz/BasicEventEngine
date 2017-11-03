/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#include "bee/engine.hpp" // Include the engine headers
#include "resources/resources.hpp" // Include the resource definitions

// Undefine main() if it has been previously defined on the Windows platform
// I think this occurs because of SDL2/SDL_main.hpp
#ifdef _WIN32
#ifdef main
#undef main
#endif // main
#endif // _WIN32

int main(int argc, char* argv[]) {
	// Declare some game options, see bee/init/gameoptions.hpp for the struct definition
	bee::GameOptions options;
	//options.is_debug_enabled = true;

	// Add a logfile to the messenger
	bee::messenger::add_log(MACRO_TO_STR(GAME_NAME) + std::string(".log"), bee::E_OUTPUT::NORMAL);

	// Initialize the game engine
	if (bee::init(argc, argv, bee::get_standard_flags(), &rm_menu, &options)) {
		bee::messenger::handle();
		return 1; // Return 1 on initialization failure
	}

	// Output initialization message
	bee::messenger::send({"engine", "init"}, bee::E_MESSAGE::INFO,
		std::string("Initialized ") + MACRO_TO_STR(GAME_NAME) + " v" +
		std::to_string(GAME_VERSION_MAJOR) + "." + std::to_string(GAME_VERSION_MINOR) + "." + std::to_string(GAME_VERSION_RELEASE)
	);

	// Run the game engine event loop
	// This loop will run until the user closes the window, the game closes itself, or the game throws an exception
	int r = bee::loop();

	// Clean up resources and quit SDL and other libraries
	bee::close();

	if (r != 0) { // If an exception was caught by the loop
		return 2; // Return 2 on exception during game loop
	}
	return 0; // Return 0 on successful run
}
