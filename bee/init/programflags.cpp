/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_INIT_PROGRAMFLAGS
#define _BEE_INIT_PROGRAMFLAGS 1

#include "programflags.hpp"

namespace bee {
	ProgramFlags::ProgramFlags() :
		longopt(),
		shortopt('\0'),
		pre_init(true),
		has_arg(no_argument),
		func(nullptr)
	{}
	ProgramFlags::ProgramFlags(std::string l, char s, bool p, int a, std::function<void (char*)> f) :
		longopt(l),
		shortopt(s),
		pre_init(p),
		has_arg(a),
		func(f)
	{}

	/*
	* get_standard_flags() - Return a list of the default ProgramFlags which can be appended by the user
	*/
	std::list<ProgramFlags*>& get_standard_flags_internal() {
		static std::list<ProgramFlags*> flag_list;
		return flag_list;
	}
	/*
	* get_standard_flags() - Return a list of the default ProgramFlags which can be appended by the user
	*/
	std::list<ProgramFlags*> get_standard_flags() {
		std::list<ProgramFlags*>& flag_list = get_standard_flags_internal();
		if (flag_list.empty()) {
			ProgramFlags* f_help = new ProgramFlags(
				"help", 'h', true, no_argument, [] (char* arg) -> void {
					std::cerr << get_usage_text();
					throw std::string();
				}
			);
			ProgramFlags* f_debug = new ProgramFlags(
				"debug", 'd', true, no_argument, [] (char* arg) -> void {
					engine.options->is_debug_enabled = true;
				}
			);
			ProgramFlags* f_dimensions = new ProgramFlags(
				"dimensions", '\0', true, required_argument, [] (char* arg) -> void {
					std::string d (arg);
					engine.width = bee_stoi(d.substr(0, d.find("x")));
					engine.height = bee_stoi(d.substr(d.find("x")+1));
					engine.options->is_resizable = false;
				}
			);
			ProgramFlags* f_fullscreen = new ProgramFlags(
				"fullscreen", 'f', true, no_argument, [] (char* arg) -> void {
					engine.options->is_fullscreen = true;
				}
			);
			ProgramFlags* f_opengl = new ProgramFlags(
				"opengl", '\0', true, no_argument, [] (char* arg) -> void {
					engine.options->renderer_type = E_RENDERER::OPENGL4;
				}
			);
			ProgramFlags* f_noassert = new ProgramFlags(
				"no-assert", '\0', true, no_argument, [] (char* arg) -> void {
					engine.options->should_assert = false;
				}
			);
			ProgramFlags* f_sdl = new ProgramFlags(
				"sdl", '\0', true, no_argument, [] (char* arg) -> void {
					engine.options->renderer_type = E_RENDERER::SDL;
				}
			);
			ProgramFlags* f_singlerun = new ProgramFlags(
				"single-run", '\0', true, no_argument, [] (char* arg) -> void {
					engine.options->single_run = true;
				}
			);
			ProgramFlags* f_windowed = new ProgramFlags(
				"windowed", 'w', true, no_argument, [] (char* arg) -> void {
					engine.options->is_fullscreen = false;
				}
			);

			flag_list = {f_help, f_debug, f_dimensions, f_fullscreen, f_opengl, f_noassert, f_sdl, f_singlerun, f_windowed};
		}
		return flag_list;
	}
	/*
	* free_standard_flags() - Return a list of the default ProgramFlags which can be appended by the user
	*/
	int free_standard_flags() {
		std::list<ProgramFlags*>& flag_list = get_standard_flags_internal();
		if (!flag_list.empty()) {
			for (auto& flag : flag_list) {
				delete flag;
			}
			flag_list.clear();
			return 0;
		}
		return 1;
	}
}

#endif // _BEE_INIT_PROGRAMFLAGS
