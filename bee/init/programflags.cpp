/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_PROGRAMFLAGS
#define BEE_INIT_PROGRAMFLAGS 1

#include <iostream>
#include <functional>

#include "programflags.hpp"

#include "info.hpp"
#include "gameoptions.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"

namespace bee {
	namespace internal {
		std::list<ProgramFlag*> flags;
	}

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

	/**
	* @param longopt the longopt to search for
	* @returns the flag from the list which matches the given longopt
	*/
	ProgramFlag* internal::get_long_flag(const std::string& longopt) {
		for (auto& flag : flags) {
			if (flag->longopt == longopt) {
				return flag;
			}
		}
		return nullptr;
	}
	/**
	* @param shortopt the shortopt to search for
	* @returns the flag from the list which matches the given shortopt
	*/
	ProgramFlag* internal::get_short_flag(char shortopt) {
		for (auto& flag : flags) {
			if (flag->shortopt == shortopt) {
				return flag;
			}
		}
		return nullptr;
	}

	/**
	* Add a program flag for post-init parsing
	* @param flag the flag to add
	*/
	void add_flag(ProgramFlag* flag) {
		internal::flags.push_back(flag);
	}
	/**
	* Handle each flag's callback
	* @param pre_init whether this function is being called before or after engine initialization
	*
	* @returns the number of flags successfully processed, the value will be negated if a flag throws an exception
	*/
	int handle_flags(bool pre_init) {
		int amount = 0;
		const size_t arg_amount = engine->argc;
		try {
			for (size_t i=1; i<arg_amount; ++i) {
				const std::string arg (engine->argv[i]);
				if (arg[0] == '-') { // If the argument is a flag
					ProgramFlag* flag = nullptr;
					if (arg[1] == '-') { // If the argument is a long flag
						flag = internal::get_long_flag(arg.substr(2));
					} else if (arg[2] == '\0') { // If the argument is a short flag
						flag = internal::get_short_flag(arg[1]);
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
					} else if (!pre_init) {
						messenger::send({"engine", "programflags"}, E_MESSAGE::WARNING, "Unknown flag: \"" + arg + "\"");
					}
				}
			}
		} catch (const std::string&) {
			return -(amount+1);
		}

		return amount;
	}

	/**
	* Initializes the default ProgramFlags.
	* @note To add more flags, use add_flag().
	*/
	void init_standard_flags() {
		if (internal::flags.empty()) {
			add_flag(new ProgramFlag(
				"help", 'h', true, E_FLAGARG::NONE, [] (const std::string& arg) {
					std::cerr << get_usage_text();
					throw std::string("help quit");
				}
			));
			add_flag(new ProgramFlag(
				"debug", 'd', true, E_FLAGARG::NONE, [] (const std::string& arg) {
					engine->options->is_debug_enabled = true;
				}
			));
			add_flag(new ProgramFlag(
				"dimensions", '\0', true, E_FLAGARG::REQUIRED, [] (const std::string& arg) {
					engine->width = bee_stoi(arg.substr(0, arg.find("x")));
					engine->height = bee_stoi(arg.substr(arg.find("x")+1));
					engine->options->is_resizable = false;
				}
			));
			add_flag(new ProgramFlag(
				"fullscreen", 'f', true, E_FLAGARG::NONE, [] (const std::string& arg) {
					engine->options->is_fullscreen = true;
				}
			));
			add_flag(new ProgramFlag(
				"no-assert", '\0', true, E_FLAGARG::NONE, [] (const std::string& arg) {
					engine->options->should_assert = false;
				}
			));
			add_flag(new ProgramFlag(
				"single-run", '\0', true, E_FLAGARG::NONE, [] (const std::string& arg) {
					engine->options->single_run = true;
				}
			));
			add_flag(new ProgramFlag(
				"windowed", 'w', true, E_FLAGARG::NONE, [] (const std::string& arg) {
					engine->options->is_fullscreen = false;
				}
			));
			add_flag(new ProgramFlag(
				"headless", '\0', true, E_FLAGARG::NONE, [] (const std::string& arg) {
					engine->options->is_headless = true;
				}
			));
		}
	}
	/**
	* Free the list of the default ProgramFlags.
	*/
	int free_standard_flags() {
		if (!internal::flags.empty()) {
			for (auto& flag : internal::flags) {
				delete flag;
			}
			internal::flags.clear();
			return 0;
		}
		return 1;
	}
}

#endif // BEE_INIT_PROGRAMFLAGS
