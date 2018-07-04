/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENGINE
#define BEE_ENGINE 1

#include <iostream>

#include <SDL2/SDL_image.h> // Include the required SDL headers
#include <SDL2/SDL_mixer.h>

#include "engine.hpp" // Include the engine headers

#include "enum.hpp"

#include "init/gameoptions.hpp"
#include "init/info.hpp"
#include "init/programflags.hpp"

#include "messenger/messenger.hpp"

#include "core/console.hpp"
#include "core/enginestate.hpp"
#include "core/rooms.hpp"

#include "input/mouse.hpp"
#include "input/kb.hpp"

#include "network/network.hpp"

#include "render/drawing.hpp"
#include "render/render.hpp"
#include "render/renderer.hpp"

#include "resource/texture.hpp"
#include "resource/sound.hpp"
#include "resource/font.hpp"
#include "resource/room.hpp"

#include "python/python.hpp"

#include "ui/ui.hpp"

#include "util/platform.hpp"
#include "util/debug.hpp"
#include "util/windefine.hpp"
#include "util/template/real.hpp"

#ifndef NDEBUG
	extern bool verify_assertions(int, char**);
	extern bool verify_assertions();
#endif

namespace bee {
	EngineState* engine = nullptr;
	bool is_initialized = false;

	int init(int argc, char** argv, Room** _first_room, const std::list<ProgramFlag*>& extra_flags, const std::list<GameOption>& extra_options) {
		engine = new EngineState(argc, argv);

		for (auto& opt : extra_options) {
			set_option(opt.name, opt.value, opt.setter);
		}

		init_standard_flags();
		for (auto& flag : extra_flags) {
			add_flag(flag);
		}
		if (handle_flags(true) < 0) {
			return 1; // Return 1 when the flags request to exit
		}

		messenger::send({"engine", "init"}, E_MESSAGE::INFO, "Initializing BasicEventEngine v" + get_engine_version().to_str());

		if (get_option("should_assert").i) {
			#ifndef NDEBUG
				if (!verify_assertions(argc, argv)) {
					messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Assertion verification failed");
					return 2; // Return 2 when assertions could not be verified
				}
			#else
				messenger::send({"engine", "init"}, E_MESSAGE::INFO, "Failed to verify assertions: compiled without debug mode");
			#endif
		}

		if (get_option("is_network_enabled").i) {
			net::init();
		}

		if (!get_option("is_headless").i) {
			int r = internal::init_sdl(); // Initialize SDL
			if (r) {
				return r; // Return any nonzero values from SDL init
			}
		}

		python::init();

		if (!is_initialized) {
			if (init_resources()) {
				messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Failed to init resources");
				return 9; // Return 9 when the resources could not be initialized
			}
		}

		engine->texture_before = new Texture();
		engine->texture_before->load_as_target(engine->width, engine->height);
		engine->texture_after = new Texture();
		engine->texture_after->load_as_target(engine->width, engine->height);

		engine->font_default = Font::add("font_default", "liberation_mono.ttf", 16);

		messenger::handle();
		console::internal::init(); // Initialize the default console commands
		messenger::handle();

		ui::load();

		if (*_first_room != nullptr) {
			if (change_room(*_first_room, false)) {
				messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Failed to load first room");
				return 10; // Return 10 when the first room could not be loaded
			}
		}

		console::internal::init_ui();

		if (handle_flags(false) < 0) {
			return 1; // Return 1 when the flags request to exit
		}
		messenger::handle();

		return 0; // Return 0 on success
	}
	int loop() {
		engine->tickstamp = get_ticks();
		engine->fps_ticks = get_ticks();

		messenger::send({"engine"}, E_MESSAGE::START, "gameloop");
		messenger::handle();

		// Register the logging system
		if (get_option("is_headless").i) {
			messenger::internal::register_protected("cmdconsole", {"engine", "commandline"}, true, [] (const MessageContents& msg) {
				bee::console::run(msg.descr);
			});
		} else {
			messenger::internal::register_protected("cmdfilter", {"engine", "commandline"}, true, [] (const MessageContents& msg) {
				if (msg.descr == "reset") {
					messenger::reset_filter();
				} else {
					messenger::add_filter(msg.descr);
				}
			});
		}

		while (!engine->quit) {
			if (engine->current_room == nullptr) {
				messenger::send({"engine"}, E_MESSAGE::ERROR, "Aborted event loop because current_room == nullptr");
				return 1;
			}

			try {
				engine->current_room->step_begin();
				engine->current_room->check_alarms();

				if (!get_option("is_headless").i) {
					internal::handle_sdl_events();
				}

				if (util::platform::has_commandline_input()) {
					engine->commandline_input.push_back("");
					std::getline(std::cin, engine->commandline_input[engine->commandline_current]);
					if (engine->commandline_input[engine->commandline_current] == "") {
						engine->commandline_input.pop_back();
					} else {
						messenger::send({"engine", "commandline"}, E_MESSAGE::INTERNAL, engine->commandline_input[engine->commandline_current]);
						engine->current_room->commandline_input(engine->commandline_input[engine->commandline_current++]);
					}
				}

				engine->current_room->step_mid();
				engine->current_room->check_paths();
				engine->current_room->outside_room();
				engine->current_room->intersect_boundary();
				engine->current_room->collision();

				engine->current_room->step_end();
				if (!get_option("is_headless").i) {
					internal::handle_drawing();
				}
				engine->current_room->destroy();

				net::handle_events();
				messenger::handle();

				internal::frame_delay();

				// If the single_run flag option is used, exit the game loop after saving a screenshot
				if (get_option("single_run").i) {
					if (!get_option("is_headless").i) {
						engine->current_room->draw();
						save_screenshot("single_run.bmp");
					}
					engine->quit = true;
				}
			} catch (int e) {
				switch (e) {
					case -1: { // Resource error
						messenger::send({"engine"}, E_MESSAGE::ERROR, "Aborting due to resource error");
						return 2;
					}
					case 0: { // Jump to loop end, e.g. change room
						break;
					}
					case 1: { // Quit
						engine->quit = true;
						break;
					}
					case 2: { // Restart game
						Sound::stop_loops(); // Stop all looping sounds from the previous run
						change_room(engine->first_room, false);
						break;
					}
					case 3: { // Restart room
						change_room(engine->current_room, false);
						break;
					}
					default: {
						messenger::send({"engine"}, E_MESSAGE::ERROR, "Unknown error code: " + std::to_string(e));
						return e;
					}
				}
			}
		}
		messenger::send({"engine"}, E_MESSAGE::END, "gameloop");

		change_room(nullptr, false);
		engine->current_room->room_end();
		engine->current_room->game_end();

		engine->current_room->reset_properties();
		engine->current_room = nullptr;
		engine->is_ready = false;

		return 0;
	}
	int close() {
		messenger::handle();

		if (is_initialized) {
			close_resources();
		}

		console::internal::close();
		ui::free();

		python::close();

		if (net::get_is_initialized()) {
			net::close();
		}

		mouse::close();
		engine->free();

		if (!get_option("is_headless").i) {
			internal::close_sdl();
		}

		free_standard_flags();

		if (engine != nullptr) {
			delete engine;
			engine = nullptr;
		}

		messenger::handle();
		messenger::clear();

		return 0;
	}

	int internal::init_sdl() {
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Failed to init SDL: " + util::get_sdl_error());
			return 3; // Return 3 when SDL could not be initialized
		}

		// Use the highest version of OpenGL available
		switch (static_cast<E_RENDERER>(get_option("renderer_type").i)) {
			case E_RENDERER::OPENGL4:
			default: {
				if (GL_VERSION_4_1) { // FIXME: Properly test for opengl support
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
					break;
				}
			}
			case E_RENDERER::OPENGL3: {
				if (GL_VERSION_3_3) {
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
					SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
					break;
				}
			}
		}

		int window_flags = SDL_WINDOW_OPENGL;
		if (get_option("is_fullscreen").i) {
			window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		if (get_option("is_borderless").i) {
			window_flags |= SDL_WINDOW_BORDERLESS;
		}
		if (get_option("is_resizable").i) {
			window_flags |= SDL_WINDOW_RESIZABLE;
		}
		if (get_option("is_maximized").i) {
			window_flags |= SDL_WINDOW_MAXIMIZED;
		}
		if (get_option("is_highdpi").i) {
			window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
		}
		if (get_option("is_visible").i) {
			window_flags |= SDL_WINDOW_SHOWN;
		}
		engine->renderer->window = SDL_CreateWindow("BasicEventEngine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, engine->width, engine->height, window_flags);
		if (engine->renderer->window == nullptr) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Failed to create SDL window: " + util::get_sdl_error());
			return 4; // Return 4 when the window could not be created
		}
		if (get_option("is_minimized").i) {
			SDL_MinimizeWindow(engine->renderer->window);
		}

		mouse::init();
		kb::init();

		if (engine->renderer->opengl_init()) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Could not initialize the OpenGL renderer");
			return 5; // Return 5 when the renderer could not be initialized
		}

		int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(img_flags) & img_flags)) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Failed to init SDL_image: " + std::string(IMG_GetError()));
			return 6; // Return 6 when SDL_image could not be initialized
		}

		if (TTF_Init() == -1) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Failed to init SDL_ttf: " + std::string(TTF_GetError()));
			return 7; // Return 7 when SDL_ttf could not be initialized
		}

		Mix_SetSoundFonts(""); // This will disable MIDI but fixes a small error when not including a soundfont
		if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Failed to init SDL_mixer: " + std::string(Mix_GetError()));
			return 8; // Return 8 when SDL_mixer could not be initialized
		}
		Mix_ChannelFinished(Sound::finished);
		Mix_AllocateChannels(128); // Probably overkill

		return 0;
	}
	int internal::handle_sdl_events() {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT: {
					engine->quit = true;
					break;
				}
				case SDL_WINDOWEVENT: {
					switch (event.window.event) {
						case SDL_WINDOWEVENT_SHOWN: {
							render::set_camera(nullptr);
							render::render();
							engine->has_focus = true;
							break;
						}
						case SDL_WINDOWEVENT_HIDDEN: {
							engine->has_focus = false;
							break;
						}
						case SDL_WINDOWEVENT_EXPOSED: {
							render::render();
							break;
						}
						case SDL_WINDOWEVENT_MOVED: {
							break;
						}
						case SDL_WINDOWEVENT_RESIZED: {
							engine->width = event.window.data1;
							engine->height = event.window.data2;
							render::set_camera(nullptr);
							render::render();
							break;
						}
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							render::render();
							break;
						}
						case SDL_WINDOWEVENT_MINIMIZED: {
							set_option("is_minimized", true);
							engine->has_mouse = false;
							engine->has_focus = false;
							break;
						}
						case SDL_WINDOWEVENT_MAXIMIZED: {
							set_option("is_minimized", false);
							engine->has_focus = true;
							break;
						}
						case SDL_WINDOWEVENT_RESTORED: {
							set_option("is_minimized", false);
							engine->has_focus = true;
							break;
						}
						case SDL_WINDOWEVENT_ENTER: {
							engine->has_mouse = true;
							break;
						}
						case SDL_WINDOWEVENT_LEAVE: {
							engine->has_mouse = false;
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_GAINED: {
							engine->has_focus = true;
							break;
						}
						case SDL_WINDOWEVENT_FOCUS_LOST: {
							engine->has_focus = false;
							break;
						}
						case SDL_WINDOWEVENT_CLOSE: {
							SDL_Event qe;
							qe.type = SDL_QUIT;
							SDL_PushEvent(&qe);
							break;
						}
						#if SDL_VERSION_ATLEAST(2, 0, 5)
							case SDL_WINDOWEVENT_TAKE_FOCUS:
								SDL_SetWindowInputFocus(engine->renderer->window);
								break;
							case SDL_WINDOWEVENT_HIT_TEST:
								break;
						#endif
						default: {
							messenger::send({"engine"}, E_MESSAGE::WARNING, "Unknown window event: " + std::to_string(event.window.event));
							break;
						}
					}
					engine->current_room->window(&event);
					break;
				}

				case SDL_KEYDOWN: {
					kb::internal::handle_input(&event);

					if (event.key.repeat == 0) {
						engine->current_room->keyboard_press(&event);
					}
					engine->current_room->keyboard_input(&event);

					break;
				}
				case SDL_KEYUP: {
					engine->current_room->keyboard_release(&event);
					break;
				}
				case SDL_MOUSEMOTION:
				case SDL_MOUSEWHEEL: {
					engine->current_room->mouse_input(&event);
					break;
				}
				case SDL_MOUSEBUTTONDOWN: {
					engine->current_room->mouse_press(&event);
					break;
				}
				case SDL_MOUSEBUTTONUP: {
					engine->current_room->mouse_release(&event);
					break;
				}
				case SDL_CONTROLLERAXISMOTION: {
					engine->current_room->controller_axis(&event);
					break;
				}
				case SDL_CONTROLLERBUTTONDOWN: {
					engine->current_room->controller_press(&event);
					break;
				}
				case SDL_CONTROLLERBUTTONUP: {
					engine->current_room->controller_release(&event);
					break;
				}
				case SDL_CONTROLLERDEVICEADDED:
				case SDL_CONTROLLERDEVICEREMOVED:
				case SDL_CONTROLLERDEVICEREMAPPED: {
					engine->current_room->controller_modify(&event);
					break;
				}

				// Android, iOS, and WinRT events
				case SDL_APP_TERMINATING:
				case SDL_APP_LOWMEMORY:
				case SDL_APP_WILLENTERBACKGROUND:
				case SDL_APP_DIDENTERBACKGROUND:
				case SDL_APP_WILLENTERFOREGROUND:
				case SDL_APP_DIDENTERFOREGROUND:
				// System specific window event
				case SDL_SYSWMEVENT:
				// Extra keyboard events
				case SDL_TEXTEDITING:
				case SDL_TEXTINPUT:
				// Joystick events
				case SDL_JOYAXISMOTION:
				case SDL_JOYBALLMOTION:
				case SDL_JOYHATMOTION:
				case SDL_JOYBUTTONDOWN:
				case SDL_JOYBUTTONUP:
				case SDL_JOYDEVICEADDED:
				case SDL_JOYDEVICEREMOVED:
				// Touch events
				case SDL_FINGERDOWN:
				case SDL_FINGERUP:
				case SDL_FINGERMOTION:
				// Gesture events
				case SDL_DOLLARGESTURE:
				case SDL_DOLLARRECORD:
				case SDL_MULTIGESTURE:
				// Clipboard event
				case SDL_CLIPBOARDUPDATE:
				// Drag and drop event
				case SDL_DROPFILE:
				// User specific event
				case SDL_USEREVENT:
				#if SDL_VERSION_ATLEAST(2, 0, 2)
					// Render event
					case SDL_RENDER_TARGETS_RESET:
				#endif
				#if SDL_VERSION_ATLEAST(2, 0, 4)
					// Extra keyboard event
					case SDL_KEYMAPCHANGED:
					// Audio hotplug events
					case SDL_AUDIODEVICEADDED:
					case SDL_AUDIODEVICEREMOVED:
					// Render event
					case SDL_RENDER_DEVICE_RESET:
				#endif
				#if SDL_VERSION_ATLEAST(2, 0, 5)
					// Drag and drop events
					case SDL_DROPTEXT:
					case SDL_DROPBEGIN:
					case SDL_DROPCOMPLETE:
				#endif
				{
					// For events which we don't care about or currently support
					break;
				}
				default:
					messenger::send({"engine"}, E_MESSAGE::WARNING, "Unknown event type: " + std::to_string(event.type));
					break;
			}
		}

		return 0;
	}
	int internal::handle_drawing() {
		engine->current_room->draw();
		engine->current_room->animation_end();
		return 0;
	}
	int internal::close_sdl() {
		Mix_AllocateChannels(0);

		Mix_CloseAudio();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();

		return 0;
	}

	int internal::update_delta() {
		engine->tick_delta = get_ticks() - engine->tickstamp;
		engine->tickstamp = get_ticks();
		return 0;
	}
	int internal::frame_delay() {
		engine->fps_count++;
		engine->frame_number++;
		unsigned int new_tickstamp = get_ticks();
		unsigned int fps_desired = std::min(engine->fps_goal, engine->fps_max);
		if (!engine->has_focus) {
			fps_desired = engine->fps_unfocused;
		}

		Uint32 frame_ticks = 1000/fps_desired;
		if (new_tickstamp - engine->tickstamp < frame_ticks) {
			if ((!get_option("is_vsync_enabled").i)||(!engine->has_focus)) {
				Uint32 delay = frame_ticks - (new_tickstamp - engine->tickstamp) + 1;
				//messenger::log("FPS delay: " + std::to_string(delay) + "ms, " + std::to_string(100*delay/frame_ticks) + "% of the frame");
				SDL_Delay(delay);
			}
		} else {
			Uint32 overtime = (new_tickstamp - engine->tickstamp) - frame_ticks;
			engine->lost_ticks += overtime;
		}

		internal::update_delta();

		// Compute the number of frames in the last second, the stable fps
		if (engine->tickstamp - engine->fps_ticks >= 1000) {
			engine->fps_stable = engine->fps_count;
			engine->fps_count = 0;
			engine->fps_ticks = engine->tickstamp;

			if (engine->lost_ticks > 5*frame_ticks) { // If the lost ticks are more than 5 frames worth, output a warning
				E_MESSAGE type = E_MESSAGE::INFO;
				if (engine->lost_ticks >= 10*frame_ticks) {
					type = E_MESSAGE::WARNING;
				}
				messenger::send({"engine"}, type, "Engine over time by " + std::to_string(engine->lost_ticks) + "ms, " + std::to_string(engine->lost_ticks/frame_ticks) + " frames lost");
			}
			engine->lost_ticks = 0;
		}

		return 0;
	}
	/**
	* @returns the millisecond ticks elapsed since initialization
	*/
	Uint32 get_ticks() {
		return SDL_GetTicks();
	}
	/**
	* @returns the seconds elapsed since initialization
	*/
	Uint32 get_seconds() {
		return get_ticks()/1000;
	}
	/**
	* @returns the frames elapsed since initialization
	*/
	Uint32 get_frame() {
		return engine->frame_number;
	}
	/**
	* @returns the seconds elapsed since last frame
	*/
	double get_delta() {
		return static_cast<double>(std::max(get_tick_delta(), 1u)) / 1000;
	}
	/**
	* @returns the millisecond ticks elapsed since last frame
	*/
	Uint32 get_tick_delta() {
		return engine->tick_delta;
	}
	/**
	* @returns the goal frames per second, e.g. 60fps
	*/
	unsigned int get_fps_goal() {
		return engine->fps_goal;
	}

	/**
	* Restart the game without reinitializaing.
	* @throws int(2) Jump to loop end and reload the first room
	*/
	void restart_game() {
		throw 2;
	}
	/**
	* End the game.
	* @throws int(1) Jump to loop end and quit
	*/
	void end_game() {
		throw 1;
	}
}

#endif // BEE_ENGINE
