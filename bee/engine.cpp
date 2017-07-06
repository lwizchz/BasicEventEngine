/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_ENGINE
#define BEE_ENGINE 1

#include <SDL2/SDL_image.h> // Include the required SDL headers
#include <SDL2/SDL_mixer.h>

#include "engine.hpp" // Include the engine headers

#include "debug.hpp"
#include "util.hpp"
#include "enum.hpp"

#include "init/gameoptions.hpp"
#include "init/programflags.hpp"

#include "messenger/messenger.hpp"

#include "core/console.hpp"
#include "core/enginestate.hpp"
#include "core/input.hpp"
#include "core/resources.hpp"
#include "core/room.hpp"

#include "network/network.hpp"

#include "render/drawing.hpp"
#include "render/render.hpp"
#include "render/renderer.hpp"

#include "resource/sprite.hpp"
#include "resource/font.hpp"
#include "resource/room.hpp"

namespace bee {
	EngineState* engine = nullptr;
	bool is_initialized = false;

	int init(int argc, char** argv, const std::list<ProgramFlags*>& new_flags, Room** new_first_room, GameOptions* new_options) {
		engine = new EngineState(argc, argv, new_options);

		if (handle_flags(new_flags, true) < 0) {
			return 1; // Return 1 when the flags request to exit
		}

		messenger::send({"engine", "init"}, E_MESSAGE::INFO,
			"Initializing with BasicEventEngine v" +
			std::to_string(BEE_VERSION_MAJOR) + "." + std::to_string(BEE_VERSION_MINOR) + "." + std::to_string(BEE_VERSION_RELEASE)
		);

		if (engine->options->should_assert) {
			if (!verify_assertions(argc, argv)) {
				messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Couldn't verify assertions");
				return 2; // Return 2 when assertions could not be verified
			}
		}

		net_init();

		if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Couldn't init SDL: " + get_sdl_error());
			return 3; // Return 3 when SDL couldn't be initialized
		}

		// Use the highest version of OpenGL available
		switch (engine->options->renderer_type) {
			case E_RENDERER::OPENGL4: {
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
			case E_RENDERER::SDL:
			default: {
				engine->options->renderer_type = E_RENDERER::SDL;
			}
		}

		int window_flags = SDL_WINDOW_OPENGL;
		if (engine->options->is_fullscreen) {
			window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}
		if (engine->options->is_borderless) {
			window_flags |= SDL_WINDOW_BORDERLESS;
		}
		if (engine->options->is_resizable) {
			window_flags |= SDL_WINDOW_RESIZABLE;
		}
		if (engine->options->is_maximized) {
			window_flags |= SDL_WINDOW_MAXIMIZED;
		}
		if (engine->options->is_highdpi) {
			window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
		}
		if (engine->options->is_visible) {
			window_flags |= SDL_WINDOW_SHOWN;
		}
		engine->renderer->window = SDL_CreateWindow("BasicEventEngine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, engine->width, engine->height, window_flags);
		if (engine->renderer->window == nullptr) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Couldn't create SDL window: " + get_sdl_error());
			return 4; // Return 4 when the window could not be created
		}
		if (engine->options->is_minimized) {
			SDL_MinimizeWindow(engine->renderer->window);
		}

		engine->cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		SDL_SetCursor(engine->cursor);

		engine->keystate = SDL_GetKeyboardState(nullptr);
		keystrings_populate();

		if (engine->options->renderer_type != E_RENDERER::SDL) {
			if (engine->renderer->opengl_init()) {
				messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Could not initialize the OpenGL renderer");
				return 5; // Return 5 when the renderer could not be initialized
			}
		} else { // if not OpenGL, init an SDL renderer
			if (engine->renderer->sdl_renderer_init()) {
				messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Could not initialized the SDL renderer");
				return 5; // Return 5 when the renderer could not be initialized
			}
		}

		int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
		if (!(IMG_Init(img_flags) & img_flags)) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Couldn't init SDL_image: " + std::string(IMG_GetError()));
			return 6; // Return 6 when SDL_image could not be initialized
		}

		if (TTF_Init() == -1) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Couldn't init SDL_ttf: " + std::string(TTF_GetError()));
			return 7; // Return 7 when SDL_ttf could not be initialized
		}

		Mix_SetSoundFonts(""); // This will disable MIDI but fixes a small error when not including a soundfont
		if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
			messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Couldn't init SDL_mixer: " + std::string(Mix_GetError()));
			return 8; // Return 8 when SDL_mixer could not be initialized
		}
		Mix_ChannelFinished(sound_finished);
		Mix_AllocateChannels(128); // Probably overkill

		if (!is_initialized) {
			if (init_resources()) {
				messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Couldn't init resources");
				return 9; // Return 9 when the resources could not be initialized
			}
		}

		engine->texture_before = new Sprite();
		engine->texture_after = new Sprite();

		engine->font_default = new Font("font_default", "liberation_mono.ttf", 16, false);
			engine->font_default->load();

		messenger::handle();
		engine->console = new Console(); // Initialize the default console commands
		internal::console_init_commands();
		messenger::handle();

		if (*new_first_room != nullptr) {
			if (change_room(*new_first_room, false)) {
				messenger::send({"engine", "init"}, E_MESSAGE::ERROR, "Couldn't load first room");
				return 10; // Return 10 when the first room could not be loaded
			}
		}

		if (handle_flags(new_flags, false) < 0) {
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
		messenger::internal::register_protected("cmdlog", {"engine", "commandline"}, true, [] (std::shared_ptr<MessageContents> msg) {
			std::cout << "[" << msg->descr << "]\n";
		});

		while (!engine->quit) {
			if (engine->current_room == nullptr) {
				messenger::send({"engine"}, E_MESSAGE::ERROR, "Aborted event loop because current_room == nullptr");
				return 1;
			}

			try {
				engine->current_room->step_begin();
				engine->current_room->check_alarms();

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
									render_set_camera(nullptr);
									engine->renderer->render();
									engine->has_focus = true;
									break;
								}
								case SDL_WINDOWEVENT_HIDDEN: {
									engine->has_focus = false;
									break;
								}
								case SDL_WINDOWEVENT_EXPOSED: {
									engine->renderer->render();
									break;
								}
								case SDL_WINDOWEVENT_MOVED: {
									break;
								}
								case SDL_WINDOWEVENT_RESIZED: {
									engine->width = event.window.data1;
									engine->height = event.window.data2;
									render_set_camera(nullptr);
									engine->renderer->render();
									break;
								}
								case SDL_WINDOWEVENT_SIZE_CHANGED: {
									engine->renderer->render();
									break;
								}
								case SDL_WINDOWEVENT_MINIMIZED: {
									engine->options->is_minimized = true;
									engine->has_mouse = false;
									engine->has_focus = false;
									break;
								}
								case SDL_WINDOWEVENT_MAXIMIZED: {
									engine->options->is_minimized = false;
									engine->has_focus = true;
									break;
								}
								case SDL_WINDOWEVENT_RESTORED: {
									engine->options->is_minimized = false;
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
									messenger::send({"engine"}, E_MESSAGE::WARNING, "Unknown window event: " + bee_itos(event.window.event));
									break;
								}
							}
							engine->current_room->window(&event);
							break;
						}

						case SDL_KEYDOWN: {
							internal::console_handle_input(&event);

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

						#if SDL_VERSION_ATLEAST(2, 0, 4)
							case SDL_AUDIODEVICEADDED:
							case SDL_AUDIODEVICEREMOVED:
						#endif
						case SDL_DOLLARGESTURE:
						case SDL_DOLLARRECORD:
						case SDL_DROPFILE:
						case SDL_FINGERMOTION:
						case SDL_FINGERDOWN:
						case SDL_FINGERUP:
						case SDL_JOYAXISMOTION:
						case SDL_JOYBALLMOTION:
						case SDL_JOYHATMOTION:
						case SDL_JOYBUTTONDOWN:
						case SDL_JOYBUTTONUP:
						case SDL_JOYDEVICEADDED:
						case SDL_JOYDEVICEREMOVED:
						case SDL_MULTIGESTURE:
						case SDL_SYSWMEVENT:
						case SDL_TEXTEDITING:
						case SDL_TEXTINPUT:
						case SDL_USEREVENT: {
							// For events which we don't care about or currently support
							break;
						}
						default:
							messenger::send({"engine"}, E_MESSAGE::WARNING, "Unknown event type: " + bee_itos(event.type));
							break;
					}
				}
				if (bee_has_commandline_input()) {
					engine->commandline_input.push_back("");
					std::getline(std::cin, engine->commandline_input[engine->commandline_current]);
					if (engine->commandline_input[engine->commandline_current] == "") {
						engine->commandline_input.pop_back();
					} else {
						messenger::send({"engine", "commandline"}, E_MESSAGE::INFO, engine->commandline_input[engine->commandline_current]);
						engine->current_room->commandline_input(engine->commandline_input[engine->commandline_current++]);
					}
				}

				engine->current_room->step_mid();
				engine->current_room->check_paths();
				engine->current_room->outside_room();
				engine->current_room->intersect_boundary();
				engine->current_room->collision();

				engine->current_room->step_end();
				engine->current_room->draw();
				engine->current_room->animation_end();
				engine->current_room->destroy();

				net_handle_events();
				messenger::handle();

				engine->fps_count++;
				engine->frame_number++;
				unsigned int new_tickstamp = get_ticks();
				unsigned int fps_desired = min<unsigned int>(engine->fps_goal, engine->fps_max);
				if (!engine->has_focus) {
					fps_desired = engine->fps_unfocused;
				}
				if (
					(new_tickstamp - engine->tickstamp < 1000/fps_desired)
					&&(new_tickstamp - engine->tickstamp > 0)
				) {
					if ((!engine->options->is_vsync_enabled)||(!engine->has_focus)) {
						Uint32 delay = (1000/fps_desired) - (new_tickstamp - engine->tickstamp);
						//messenger::log("FPS delay: " + bee_itos(delay) + "ms, " + bee_itos(100*delay/fps_desired) + "% of the frame");
						SDL_Delay(delay);
					}
				} else if (new_tickstamp - engine->tickstamp > 3*1000/fps_desired) { // If the tick difference is more than 3 frames worth, output a warning
					Uint32 overtime = (new_tickstamp - engine->tickstamp) - (1000/fps_desired);
					messenger::send({"engine"}, E_MESSAGE::WARNING, "Engine loop over time by " + bee_itos(overtime) + "ms, " + bee_itos(overtime/(1000/fps_desired)) + " frames lost");
				}
				internal::update_delta();

				// Compute the number of frames in the last second, the stable fps
				if (engine->tickstamp - engine->fps_ticks >= 1000) {
					engine->fps_stable = engine->fps_count / ((engine->tickstamp-engine->fps_ticks)/1000);
					engine->fps_count = 0;
					engine->fps_ticks = engine->tickstamp;
				}

				// If the single_run flag option is used, exit the game loop after saving a screenshot
				if (engine->options->single_run) {
					engine->current_room->draw();
					save_screenshot("single_run.bmp");
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
						change_room(engine->first_room, false);
						break;
					}
					case 3: { // Restart room
						change_room(engine->current_room, false);
						break;
					}
					default: {
						messenger::send({"engine"}, E_MESSAGE::ERROR, "Unknown error code: " + bee_itos(e));
						return e;
					}
				}
			} catch (...) {
				close();

				messenger::internal::send_urgent(std::shared_ptr<MessageContents>(new MessageContents(
					get_ticks(),
					{"engine"},
					E_MESSAGE::ERROR,
					"Unknown error",
					nullptr
				)));

				std::rethrow_exception(std::current_exception());
			}
		}
		messenger::send({"engine"}, E_MESSAGE::END, "gameloop");

		engine->current_room->game_end();
		change_room(nullptr, false);
		engine->is_ready = false;

		return 0;
	}
	int close() {
		messenger::handle();

		Mix_AllocateChannels(0);

		if (is_initialized) {
			free_media();
			close_resources();
		}

		engine->free();

		Mix_CloseAudio();
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();

		free_standard_flags();

		messenger::handle();
		messenger::clear();

		if (engine != nullptr) {
			delete engine;
			engine = nullptr;
		}

		return 0;
	}

	int internal::update_delta() {
		engine->tick_delta = get_ticks() - engine->tickstamp;
		engine->tickstamp = get_ticks();
		return 0;
	}
	Uint32 get_ticks() {
		return SDL_GetTicks();
	}
	Uint32 get_seconds() {
		return get_ticks()/1000;
	}
	Uint32 get_frame() {
		return engine->frame_number;
	}
	double get_delta() {
		return max<double>((double)get_tick_delta(), 1.0) / 1000.0;
	}
	Uint32 get_tick_delta() {
		return engine->tick_delta;
	}
	unsigned int get_fps_goal() {
		return engine->fps_goal;
	}

	int restart_game() {
		throw 2;
		return 0;
	}
	int end_game() {
		throw 1;
		return 0;
	}
}

#endif // BEE_ENGINE
