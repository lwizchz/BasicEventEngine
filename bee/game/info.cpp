/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_INFO
#define _BEE_GAME_INFO 1

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

/*
* BEE::get_standard_flags() - Return a list of the default ProgramFlags which can be appended by the user
*/
std::list<BEE::ProgramFlags*> BEE::get_standard_flags() {
	static std::list<BEE::ProgramFlags*> flag_list;
	if (flag_list.empty()) {
		ProgramFlags* f_help = new ProgramFlags(
			"help", 'h', true, no_argument, [] (BEE* g, char* arg) -> void {
				std::cerr << g->get_usage_text();
				throw std::string();
			}
		);
		ProgramFlags* f_debug = new ProgramFlags(
			"debug", 'd', true, no_argument, [] (BEE* g, char* arg) -> void {
				g->options->is_debug_enabled = true;
			}
		);
		ProgramFlags* f_dimensions = new ProgramFlags(
			"dimensions", '\0', true, required_argument, [] (BEE* g, char* arg) -> void {
				std::string d (arg);
				g->width = bee_stoi(d.substr(0, d.find("x")));
				g->height = bee_stoi(d.substr(d.find("x")+1));
				g->options->is_resizable = false;
			}
		);
		ProgramFlags* f_fullscreen = new ProgramFlags(
			"fullscreen", 'f', true, no_argument, [] (BEE* g, char* arg) -> void {
				g->options->is_fullscreen = true;
			}
		);
		ProgramFlags* f_opengl = new ProgramFlags(
			"opengl", '\0', true, no_argument, [] (BEE* g, char* arg) -> void {
				g->options->renderer_type = BEE_RENDERER_OPENGL4;
			}
		);
		ProgramFlags* f_noassert = new ProgramFlags(
			"no-assert", '\0', true, no_argument, [] (BEE* g, char* arg) -> void {
				g->options->should_assert = false;
			}
		);
		ProgramFlags* f_sdl = new ProgramFlags(
			"sdl", '\0', true, no_argument, [] (BEE* g, char* arg) -> void {
				g->options->renderer_type = BEE_RENDERER_SDL;
			}
		);
		ProgramFlags* f_singlerun = new ProgramFlags(
			"single-run", '\0', true, no_argument, [] (BEE* g, char* arg) -> void {
				g->options->single_run = true;
			}
		);
		ProgramFlags* f_windowed = new ProgramFlags(
			"windowed", 'w', true, no_argument, [] (BEE* g, char* arg) -> void {
				g->options->is_fullscreen = false;
			}
		);

		flag_list = {f_help, f_debug, f_dimensions, f_fullscreen, f_opengl, f_noassert, f_sdl, f_singlerun, f_windowed};
	}
	return flag_list;
}

#endif // _BEE_GAME_INFO
