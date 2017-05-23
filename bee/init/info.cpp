/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_INIT_INFO
#define _BEE_INIT_INFO 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::get_usage_test() - Return an information string about how to run the program
*/
std::string BEE::get_usage_text() {
	return
		bee_get_path() + "\n"
		"A basic event-driven game engine which renders with either SDL or OpenGL\n"
		"\n"
		"Usage:\n"
		"	" + file_basename(bee_get_path()) + " [OPTION]...\n"
		"Options:\n"
		"	--help, -h\n"
		"		Output this help text\n"
		"	--debug, -d\n"
		"		Enable debug mode for extra output and wireframes\n"
		"	--dimensions wxh\n"
		"		Set the dimensions of the window to width w and height h\n"
		"	--fullscreen, -f\n"
		"		Enable fullscreen mode, this will resize the window to fit the screen\n"
		"	--opengl, -f\n"
		"		Use the highest version of OpenGL renderer available (currently either 4.1 or 3.3)\n"
		"	--no-assert\n"
		"		Disable assertion verification for the utility functions\n"
		"	--sdl\n"
		"		Use the SDL renderer\n"
		"	--single-run\n"
		"		Run the main loop a single time to verify that things are in working order\n"
		"	--windowed, -w\n"
		"		Disable fullscreen mode\n"
		"Exit Status:\n"
		"	0       Success\n"
		"	1       Exception during engine initialization\n"
	;
}

#endif // _BEE_INIT_INFO
