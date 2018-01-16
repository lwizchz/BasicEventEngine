/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_INFO
#define BEE_INIT_INFO 1

#include "info.hpp"

#include "../util/platform.hpp"
#include "../util/files.hpp"

namespace bee {
	/*
	* get_usage_test() - Return an information string about how to run the program
	*/
	std::string get_usage_text() {
		return
			bee_get_path() + "\n"
			"A basic event-driven OpenGL game engine\n"
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
			"	--no-assert\n"
			"		Disable assertion verification for the utility functions\n"
			"	--single-run\n"
			"		Run the main loop a single time to verify assertions and initialization\n"
			"	--windowed, -w\n"
			"		Disable fullscreen mode\n"
			"	--headless\n"
			"		Run the engine in headless mode without any SDL/OpenGL initialization\n"
			"	Other options may be added prior to initialization."
			"Exit Status:\n"
			"	0       Success\n"
			"	1       Failure to initialize the engine\n"
			"	2       Unknown exception during game loop\n"
		;
	}
}

#endif // BEE_INIT_INFO
