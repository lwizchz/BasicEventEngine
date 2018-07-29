/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_INIT_GAMEOPTIONS
#define BEE_INIT_GAMEOPTIONS 1

#include <map>
#include <vector>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

#include "gameoptions.hpp"

#include "../engine.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/window.hpp"

#include "../input/mouse.hpp"

#include "../network/network.hpp"

#include "../render/renderer.hpp"

#ifndef NDEBUG
	#define SHOULD_ASSERT_DEFAULT true
#else
	#define SHOULD_ASSERT_DEFAULT false
#endif

namespace bee {
	namespace internal {
		std::map<std::string,GameOption> options = {
			{"is_fullscreen", {"is_fullscreen", Variant(true), [] (GameOption* self, Variant value) {
				self->value = value;

				if (engine->renderer->window != nullptr) {
					const std::pair<int,int> mpos (mouse::get_display_pos()); // Store the mouse position so it can be reset after changing video mode

					Uint32 f = 0;
					if (self->value.i) {
						f = SDL_WINDOW_FULLSCREEN_DESKTOP;
					}
					SDL_SetWindowFullscreen(engine->renderer->window, f);

					mouse::set_display_pos(mpos.first, mpos.second); // Reset the mouse position
				}

				return 0;
			}}},
			{"is_borderless", {"is_borderless", Variant(true), [] (GameOption* self, Variant value) {
				self->value = value;

				SDL_bool b = SDL_TRUE;
				if (self->value.i) {
					b = SDL_FALSE;
				}
				SDL_SetWindowBordered(engine->renderer->window, b);

				return 0;
			}}},
			{"is_resizable", {"is_resizable", Variant(true), [] (GameOption* self, Variant value) {
				self->value = value;

				if (self->value.i) {
					SDL_SetWindowMaximumSize(engine->renderer->window, 16384, 16384);
					SDL_SetWindowMinimumSize(engine->renderer->window, 128, 128);
				} else {
					SDL_SetWindowMaximumSize(engine->renderer->window, get_window().w, get_window().h);
					SDL_SetWindowMinimumSize(engine->renderer->window, get_window().w, get_window().h);
				}

				return 0;
			}}},
			{"is_maximized", {"is_maximized", Variant(true), [] (GameOption* self, Variant value) {
				self->value = value;

				if (self->value.i) {
					SDL_MaximizeWindow(engine->renderer->window);
				} else {
					if (get_option("is_resizable").i) {
						SDL_RestoreWindow(engine->renderer->window);
					} else {
						SDL_MinimizeWindow(engine->renderer->window);
					}
				}

				return 0;
			}}},
			{"is_highdpi", {"is_highdpi", Variant(false), [] (GameOption* self, Variant value) {
				if (engine->current_room != nullptr) {
					messenger::send({"engine", "options"}, E_MESSAGE::WARNING, "Cannot modify GameOption is_highdpi after initialization");
					return 1;
				}

				return 0;
			}}},
			{"is_visible", {"is_visible", Variant(true), [] (GameOption* self, Variant value) {
				self->value = value;

				if (self->value.i) {
					SDL_ShowWindow(engine->renderer->window);
				} else {
					SDL_HideWindow(engine->renderer->window);
				}

				return 0;
			}}},
			{"is_minimized", {"is_minimized", Variant(false), [] (GameOption* self, Variant value) {
				self->value = value;

				if (self->value.i) {
					SDL_MinimizeWindow(engine->renderer->window);
				} else {
					SDL_RestoreWindow(engine->renderer->window);
				}

				return 0;
			}}},

			{"renderer_type", {"renderer_type", Variant(static_cast<int>(E_RENDERER::OPENGL3)), [] (GameOption* self, Variant value) {
				self->value = value;

				if (engine->renderer->context != nullptr) {
					engine->renderer->reset();
				}

				return 0;
			}}},
			{"is_vsync_enabled", {"is_vsync_enabled", Variant(false), [] (GameOption* self, Variant value) {
				self->value = value;

				if (engine->renderer->context != nullptr) {
					engine->renderer->reset();
				}

				return 0;
			}}},
			{"is_basic_shaders_enabled", {"is_basic_shaders_enabled", Variant(false), [] (GameOption* self, Variant value) {
				self->value = value;

				if (engine->renderer->context != nullptr) {
					engine->renderer->reset();
				}

				return 0;
			}}},

			{"is_network_enabled", {"is_network_enabled", Variant(false), [] (GameOption* self, Variant value) {
				self->value = value;

				if (self->value.i) {
					net::init();
				} else {
					net::close();
				}

				return 0;
			}}},
			{"is_debug_enabled", {"is_debug_enabled", Variant(false), [] (GameOption* self, Variant value) {
				self->value = value;
				return 0;
			}}},

			{"should_assert", {"should_assert", Variant(SHOULD_ASSERT_DEFAULT), [] (GameOption* self, Variant value) {
				if (engine->current_room != nullptr) {
					messenger::send({"engine", "options"}, E_MESSAGE::WARNING, "Cannot modify GameOption should_assert after initialization");
					return 1;
				}

				self->value = value;
				return 0;
			}}},
			{"single_run", {"single_run", Variant(false), [] (GameOption* self, Variant value) {
				if (engine->current_room != nullptr) {
					messenger::send({"engine", "options"}, E_MESSAGE::WARNING, "Cannot modify GameOption single_run after initialization");
					return 1;
				}

				self->value = value;
				return 0;
			}}},
			{"is_headless", {"is_headless", Variant(false), [] (GameOption* self, Variant value) {
				if (engine->current_room != nullptr) {
					messenger::send({"engine", "options"}, E_MESSAGE::WARNING, "Cannot modify GameOption is_headless after initialization");
					return 1;
				}

				self->value = value;
				return 0;
			}}}
		};
	}

	GameOption::GameOption(const std::string& _name, Variant _value) :
		GameOption(_name, _value, nullptr)
	{}
	GameOption::GameOption(const std::string& _name, Variant _value, std::function<int (GameOption*, Variant)> _setter) :
		name(_name),
		value(_value),
		setter(_setter)
	{}

	/**
	* @param new_value the value to assign to this option
	*
	* @retval 0 success
	* @retval nonzero unable to change the option value
	*/
	int GameOption::set(Variant new_value) {
		if (setter != nullptr) {
			return setter(this, new_value);
		} else {
			value = new_value;
			return 0;
		}
	}

	/**
	* @returns the option value or an empty value if the option does not exist
	*/
	Variant get_option(const std::string& name) {
		if (internal::options.find(name) != internal::options.end()) {
			return internal::options.at(name).value;
		}
		return Variant();
	}
	/**
	* @param name the name of the option to set
	* @param value the new value to set the option to
	*
	* @retval 0 success
	* @retval nonzero unable to change the option value
	*/
	int set_option(const std::string& name, Variant value) {
		if (internal::options.find(name) != internal::options.end()) {
			return internal::options.at(name).set(value);
		}

		internal::options.emplace(name, GameOption(name, value));

		return 0;
	}
	/**
	* @param name the name of the option to set
	* @param value the new value to set the option to
	* @param setter the new setter to use for the option
	*
	* @retval 0 success
	* @retval nonzero unable to change the option value
	*/
	int set_option(const std::string& name, Variant value, std::function<int (GameOption*, Variant)> setter) {
		if (internal::options.find(name) != internal::options.end()) {
			int r = set_option(name, value);
			if (r == 0) {
				internal::options.at(name).setter = setter;
			}
			return r;
		}

		internal::options.emplace(name, GameOption(name, value, setter));

		return 0;
	}
}

#endif // BEE_INIT_GAMEOPTIONS
