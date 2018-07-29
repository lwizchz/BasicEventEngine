/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_INFO
#define BEE_INIT_INFO 1

#include "info.hpp"

#include "../../defines.hpp"

#include "../defines.hpp"

#include "../util/platform.hpp"
#include "../util/files.hpp"

#define BEE_VERSION_MAJOR 0
#define BEE_VERSION_MINOR 1
#define BEE_VERSION_PATCH 6

namespace bee {
	/**
	* @returns the version as a string in the format "Major.Minor.Patch"
	*/
	std::string VersionInfo::to_str() const {
		return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
	}

	/**
	* @returns information about how to run the program
	*/
	std::string get_usage_text() {
		return
			util::platform::get_path() + "\n"
			"A basic event-driven OpenGL game engine\n"
			"\n"
			"Usage:\n"
			"	" + util::file_basename(util::platform::get_path()) + " [OPTION]...\n"
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
			"	Other options may be added prior to initialization.\n"
			"Exit Status:\n"
			"	0       Success\n"
			"	1       Failure to initialize the engine\n"
			"	2       Unknown exception during game loop\n"
		;
	}

	/**
	* @returns the full build ID as a hex string
	*/
	std::string get_build_id() {
		return std::string(MACRO_TO_STR(BEE_BUILD_ID));
	}
	/**
	* @returns the game ID, which is a truncated version of the build ID
	*/
	unsigned int get_game_id() {
		return BEE_GAME_ID;
	}
	/**
	* @returns the game name as a string
	*/
	std::string get_game_name() {
		return std::string(MACRO_TO_STR(GAME_NAME));
	}

	/**
	* @returns the engine version in a struct
	*/
	VersionInfo get_engine_version() {
		return {BEE_VERSION_MAJOR, BEE_VERSION_MINOR, BEE_VERSION_PATCH};
	}
	/**
	* @returns the game version in a struct
	*/
	VersionInfo get_game_version() {
		return {GAME_VERSION_MAJOR, GAME_VERSION_MINOR, GAME_VERSION_PATCH};
	}
}

#endif // BEE_INIT_INFO
