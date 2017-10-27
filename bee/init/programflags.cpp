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
#include <map>
#include <vector>
#include <functional>

#include <SDL2/SDL.h> // Include the required SDL headers

#include "programflags.hpp"

#include "info.hpp"
#include "gameoptions.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"

namespace bee {
	ProgramFlag::ProgramFlag() :
		longopt(),
		shortopt('\0'),
		pre_init(true),
		arg_type(E_FLAGARG::NONE),
		func(nullptr)
	{}
	ProgramFlag::ProgramFlag(const std::string& l, char s, bool p, E_FLAGARG a, const std::function<void (const std::string&)>& f) :
		longopt(l),
		shortopt(s),
		pre_init(p),
		arg_type(a),
		func(f)
	{}

	/*
	* internal::get_standard_flags() - Return the static list of the default ProgramFlags
	*/
	std::list<ProgramFlag*>& internal::get_standard_flags() {
		static std::list<ProgramFlag*> flag_list;
		return flag_list;
	}
	/*
	* internal::get_long_flag() - Return the flag from the list which matches the given longopt
	* @flags: the list of flags to search
	* @longopt: the longopt to search for
	*/
	ProgramFlag* internal::get_long_flag(const std::list<ProgramFlag*>& flags, const std::string& longopt) {
		for (auto& flag : flags) {
			if (flag->longopt == longopt) {
				return flag;
			}
		}
		return nullptr;
	}
	/*
	* internal::get_short_flag() - Return the flag from the list which matches the given shortopt
	* @flags: the list of flags to search
	* @shortopt: the shortopt to search for
	*/
	ProgramFlag* internal::get_short_flag(const std::list<ProgramFlag*>& flags, char shortopt) {
		for (auto& flag : flags) {
			if (flag->shortopt == shortopt) {
				return flag;
			}
		}
		return nullptr;
	}

	/*
	* handle_flags() - Handle each flag's callback
	* @flags: the new flags to use
	* @pre_init: whether this function is being called before or after engine initialization
	*/
	int handle_flags(const std::list<ProgramFlag*>& flags, bool pre_init) {
		engine->flags = flags;

		if (engine->flags.empty()) {
			return 0;
		}

		int amount = 0;
		const size_t arg_amount = engine->argc;
		try {
			for (size_t i=1; i<arg_amount; ++i) {
				if (engine->argv[i][0] == '-') { // If the argument is a flag
					std::string arg (engine->argv[i]);
					ProgramFlag* flag = nullptr;
					if (arg[1] == '-') { // If the argument is a long flag
						flag = internal::get_long_flag(engine->flags, arg.substr(2));
					} else if (arg[2] == '\0') { // If the argument is a short flag
						flag = internal::get_short_flag(engine->flags, arg[1]);
					}

					if (flag != nullptr) {
						std::string optarg;
						if (flag->arg_type != E_FLAGARG::NONE) {
							if ((i+1 < arg_amount)&&(engine->argv[i+1][0] != '-')) {
								++i;
								optarg = std::string(engine->argv[i]);
							}
						}

						if (flag->pre_init == pre_init) {
							if (flag->func != nullptr) {
								if (flag->arg_type != E_FLAGARG::NONE) {
									if ((flag->arg_type == E_FLAGARG::REQUIRED)&&(optarg.empty())) {
										throw std::string("Missing required flag for " + arg);
									}

									flag->func(optarg);
								} else {
									flag->func(std::string());
								}
							}
							amount++;
						}
					} else {
						messenger::send({"engine", "programflags"}, E_MESSAGE::WARNING, "Unknown flag: \"" + arg + "\"");
					}
				}
			}
		} catch (const std::string&) {
			return -(amount+1);
		}

		return amount;
	}

	/*
	* get_standard_flags() - Return a list of the default ProgramFlags which can be appended by the user
	*/
	std::list<ProgramFlag*> get_standard_flags() {
		std::list<ProgramFlag*>& flag_list = internal::get_standard_flags();
		if (flag_list.empty()) {
			ProgramFlag* f_help = new ProgramFlag(
				"help", 'h', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					std::cerr << get_usage_text();
					throw std::string("help quit");
				}
			);
			ProgramFlag* f_debug = new ProgramFlag(
				"debug", 'd', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					engine->options->is_debug_enabled = true;
				}
			);
			ProgramFlag* f_dimensions = new ProgramFlag(
				"dimensions", '\0', true, E_FLAGARG::REQUIRED, [] (const std::string& arg) -> void {
					engine->width = bee_stoi(arg.substr(0, arg.find("x")));
					engine->height = bee_stoi(arg.substr(arg.find("x")+1));
					engine->options->is_resizable = false;
				}
			);
			ProgramFlag* f_fullscreen = new ProgramFlag(
				"fullscreen", 'f', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					engine->options->is_fullscreen = true;
				}
			);
			ProgramFlag* f_opengl = new ProgramFlag(
				"opengl", '\0', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					engine->options->renderer_type = E_RENDERER::OPENGL4;
				}
			);
			ProgramFlag* f_noassert = new ProgramFlag(
				"no-assert", '\0', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					engine->options->should_assert = false;
				}
			);
			ProgramFlag* f_sdl = new ProgramFlag(
				"sdl", '\0', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					engine->options->renderer_type = E_RENDERER::SDL;
				}
			);
			ProgramFlag* f_singlerun = new ProgramFlag(
				"single-run", '\0', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					engine->options->single_run = true;
				}
			);
			ProgramFlag* f_windowed = new ProgramFlag(
				"windowed", 'w', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					engine->options->is_fullscreen = false;
				}
			);
			ProgramFlag* f_headless = new ProgramFlag(
				"headless", '\0', true, E_FLAGARG::NONE, [] (const std::string& arg) -> void {
					engine->options->is_headless = true;
				}
			);

			flag_list = {f_help, f_debug, f_dimensions, f_fullscreen, f_opengl, f_noassert, f_sdl, f_singlerun, f_windowed, f_headless};
		}
		return flag_list;
	}
	/*
	* free_standard_flags() - Return a list of the default ProgramFlags which can be appended by the user
	*/
	int free_standard_flags() {
		std::list<ProgramFlag*>& flag_list = internal::get_standard_flags();
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
