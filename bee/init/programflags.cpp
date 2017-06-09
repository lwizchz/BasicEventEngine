/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_PROGRAMFLAGS
#define BEE_INIT_PROGRAMFLAGS 1

#include <iostream>
#include <getopt.h>
#include <map>
#include <vector>
#include <functional>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "programflags.hpp"

#include "info.cpp"
#include "gameoptions.hpp"

#include "../util/platform.hpp"

#include "../core/enginestate.hpp"

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

	int handle_flags(const std::list<ProgramFlags*>& new_flags, bool pre_init) {
		engine->flags = new_flags;

		if (engine->flags.empty()) {
			return 0;
		}

		int l = 0;
		struct option* long_options = new struct option[engine->flags.size()+1];
		std::string optstring = "";
		for (auto& f : engine->flags) {
			if (f->shortopt > 0) {
				optstring += f->shortopt;
				if (f->has_arg == optional_argument) {
					optstring += "::";
				} else if (f->has_arg == required_argument) {
					optstring += ":";
				}
			}

			long_options[l].name = f->longopt.c_str();
			long_options[l].has_arg = f->has_arg;
			long_options[l].flag = nullptr;
			long_options[l].val = f->shortopt;
			l++;
		}
		long_options[l++] = {0, 0, 0, 0};

		optind = 1;
		int index = -1;
		int c = -1;
		int amount = 0;
		try {
			while ((c = getopt_long(engine->argc, engine->argv, optstring.c_str(), long_options, &index)) != -1) {
				for (auto& f : engine->flags) {
					if (((c != 0)&&(c == f->shortopt))||((c == 0)&&(strcmp(long_options[index].name, f->longopt.c_str()) == 0))) {
						if (f->pre_init == pre_init) {
							if (f->func != nullptr) {
								if ((f->has_arg != no_argument)&&(optarg)) {
									f->func(optarg);
								} else {
									f->func((char*)nullptr);
								}
							}
							amount++;
						}
						break;
					}
				}
			}
		} catch(std::string e) {
			delete[] long_options;
			return -(amount+1);
		}

		delete[] long_options;

		return amount;
	}
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
					throw std::string("help quit");
				}
			);
			ProgramFlags* f_debug = new ProgramFlags(
				"debug", 'd', true, no_argument, [] (char* arg) -> void {
					engine->options->is_debug_enabled = true;
				}
			);
			ProgramFlags* f_dimensions = new ProgramFlags(
				"dimensions", '\0', true, required_argument, [] (char* arg) -> void {
					std::string d (arg);
					engine->width = bee_stoi(d.substr(0, d.find("x")));
					engine->height = bee_stoi(d.substr(d.find("x")+1));
					engine->options->is_resizable = false;
				}
			);
			ProgramFlags* f_fullscreen = new ProgramFlags(
				"fullscreen", 'f', true, no_argument, [] (char* arg) -> void {
					engine->options->is_fullscreen = true;
				}
			);
			ProgramFlags* f_opengl = new ProgramFlags(
				"opengl", '\0', true, no_argument, [] (char* arg) -> void {
					engine->options->renderer_type = E_RENDERER::OPENGL4;
				}
			);
			ProgramFlags* f_noassert = new ProgramFlags(
				"no-assert", '\0', true, no_argument, [] (char* arg) -> void {
					engine->options->should_assert = false;
				}
			);
			ProgramFlags* f_sdl = new ProgramFlags(
				"sdl", '\0', true, no_argument, [] (char* arg) -> void {
					engine->options->renderer_type = E_RENDERER::SDL;
				}
			);
			ProgramFlags* f_singlerun = new ProgramFlags(
				"single-run", '\0', true, no_argument, [] (char* arg) -> void {
					engine->options->single_run = true;
				}
			);
			ProgramFlags* f_windowed = new ProgramFlags(
				"windowed", 'w', true, no_argument, [] (char* arg) -> void {
					engine->options->is_fullscreen = false;
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

#endif // BEE_INIT_PROGRAMFLAGS
