/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME
#define _BEE_GAME 1

#include "game.hpp"

MetaResourceList* BEE::resource_list;
bool BEE::is_initialized = false;

BEE::BEE(int new_argc, char** new_argv, Room** new_first_room, GameOptions* new_options) {
	argc = new_argc;
	argv = new_argv;
	is_ready = false;
	options = new_options;

	is_minimized = false;
	is_fullscreen = false;
	has_mouse = false;
	has_focus = false;

	fps_goal = DEFAULT_GAME_FPS;
	//fps_max = 300;
	fps_max = fps_goal;
	fps_unfocused = fps_max/20;
	fps_count = 0;
	fps_stable = 0;

	width = DEFAULT_WINDOW_WIDTH;
	height = DEFAULT_WINDOW_HEIGHT;

	net = new NetworkData();

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
		throw std::string("Couldn't init SDL: ") + SDL_GetError() + "\n";
	}

	int window_flags = SDL_WINDOW_SHOWN;
	if (options->is_fullscreen) {
		if (platform == 0) {
		       //if (options->is_resizable) {
			       window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; // Changes the window dimensions
		       /*} else {
			       window_flags |= SDL_WINDOW_FULLSCREEN; // Changes the video mode
		       }*/
		} else if (platform == 1) { // _WINDOWS
			window_flags |= SDL_WINDOW_BORDERLESS;
		}
	}
	if (options->is_opengl) {
		window_flags |= SDL_WINDOW_OPENGL;
	}
	if (options->is_borderless) {
		window_flags |= SDL_WINDOW_BORDERLESS;
	}
	if (options->is_resizable) {
		window_flags |= SDL_WINDOW_RESIZABLE;
	}
	if (options->is_maximized) {
		window_flags |= SDL_WINDOW_MAXIMIZED;
	}
	if (options->is_highdpi) {
		window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
	}
	window = SDL_CreateWindow("Basic Event Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);
	if (window == NULL) {
		throw std::string("Couldn't create SDL window: ") + SDL_GetError() + "\n";
	}

	cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	SDL_SetCursor(cursor);

	keystate = SDL_GetKeyboardState(NULL);

	if (options->is_opengl) {
		// Do OpenGL stuff
	} else {
		int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
		if (options->is_vsync_enabled) {
			renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
		}
		renderer = SDL_CreateRenderer(window, -1, renderer_flags);
		if (renderer == NULL) {
			throw std::string("Couldn't create SDL renderer: ") + SDL_GetError() + "\n";
		}
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	}

	int img_flags = IMG_INIT_PNG | IMG_INIT_JPG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		throw std::string("Couldn't init SDL_image: ") + IMG_GetError() + "\n";
	}

	if (TTF_Init() == -1) {
		throw std::string("Couldn't init SDL_ttf: ") + TTF_GetError() + "\n";
	}

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 1024) < 0) {
		throw std::string("Couldn't init SDL_mixer: ") + Mix_GetError() + "\n";
	}
	Mix_ChannelFinished(sound_finished);
	Mix_AllocateChannels(128); // Probably overkill

	if (options->is_network_enabled) {
		if (!network_init()) {
			net->is_initialized = true;
		}
	}

	if (!is_initialized) {
		resource_list = new MetaResourceList();
		if (init_resources()) {
			throw std::string("Couldn't init resources\n");
		}
	}

	if (set_engine_pointer()) {
		throw std::string("Couldn't set engine pointer\n");
	}

	texture_before = new Sprite();
	texture_before->game = this;
	texture_after = new Sprite();
	texture_after->game = this;

	quit = false;
	if (*new_first_room != NULL) {
		if (change_room(*new_first_room, false)) {
			throw std::string("Couldn't load first room\n");
		}
	}
}
BEE::~BEE() {
	close();
}
int BEE::loop() {
	if (current_room == NULL) {
		throw std::string("Failed to start event loop, i.e. current_room == NULL\n");
	}

	tickstamp = get_ticks();
	fps_ticks = get_ticks();
	while (!quit) {
		try {
			current_room->step_begin();
			current_room->check_alarms();

			while (SDL_PollEvent(&event)) {
				switch (event.type) {
					case SDL_QUIT: {
						quit = true;
						break;
					}
					case SDL_WINDOWEVENT: {
						switch (event.window.event) {
							case SDL_WINDOWEVENT_SHOWN: {
								//render_reset();
								render();
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_HIDDEN: {
								has_focus = false;
								break;
							}
							case SDL_WINDOWEVENT_EXPOSED: {
								render();
								break;
							}
							case SDL_WINDOWEVENT_MOVED: {
								break;
							}
							case SDL_WINDOWEVENT_RESIZED: {
								width = event.window.data1;
								height = event.window.data2;
								render();
								break;
							}
							case SDL_WINDOWEVENT_SIZE_CHANGED: {
								render();
								break;
							}
							case SDL_WINDOWEVENT_MINIMIZED: {
								is_minimized = true;
								has_mouse = false;
								has_focus = false;
								break;
							}
							case SDL_WINDOWEVENT_MAXIMIZED: {
								is_minimized = false;
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_RESTORED: {
								is_minimized = false;
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_ENTER: {
								has_mouse = true;
								break;
							}
							case SDL_WINDOWEVENT_LEAVE: {
								has_mouse = false;
								break;
							}
							case SDL_WINDOWEVENT_FOCUS_GAINED: {
								has_focus = true;
								break;
							}
							case SDL_WINDOWEVENT_FOCUS_LOST: {
								has_focus = false;
								break;
							}
							case SDL_WINDOWEVENT_CLOSE: {
								SDL_Event qe;
								qe.type = SDL_QUIT;
								SDL_PushEvent(&qe);
								break;
							}
							default: {
								std::cerr << "other,";
							}
						}
						current_room->window(&event);
						break;
					}

					case SDL_KEYDOWN: {
						if (event.key.repeat == 0) {
							current_room->keyboard_press(&event);
						}
						current_room->keyboard_input(&event);
						break;
					}
					case SDL_KEYUP: {
						current_room->keyboard_release(&event);
						break;
					}
					case SDL_MOUSEMOTION:
					case SDL_MOUSEWHEEL: {
						current_room->mouse_input(&event);
						break;
					}
					case SDL_MOUSEBUTTONDOWN: {
						current_room->mouse_press(&event);
						break;
					}
					case SDL_MOUSEBUTTONUP: {
						current_room->mouse_release(&event);
						break;
					}
					default:
						//std::cerr << "Unknown event type: " << event.type << "\n";
						break;
				}
			}
			current_room->keyboard();
			current_room->mouse();

			current_room->step_mid();
			current_room->check_paths();
			current_room->outside_room();
			current_room->intersect_boundary();
			current_room->collision();

			current_room->step_end();
			current_room->draw();
			current_room->animation_end();
			current_room->destroy();

			net_handle_events();

			fps_count++;
			frame_number++;
			new_tickstamp = get_ticks();
			unsigned int fps_desired = fps_max;
			if (!has_focus) {
				fps_desired = fps_unfocused;
			}
			if (new_tickstamp - tickstamp < 1000/fps_desired) {
				if ((!options->is_vsync_enabled)||(!has_focus)) {
					SDL_Delay((1000/fps_desired) - (new_tickstamp - tickstamp));
				}
			}
			tickstamp = get_ticks();

			if (tickstamp - fps_ticks >= 1000) {
				fps_stable = fps_count / ((tickstamp-fps_ticks)/1000);
				fps_count = 0;
				fps_ticks = tickstamp;
			}
		} catch (int e) {
			switch (e) {
				case -1: { // Resource error
					throw std::string("Aborting due to resource error\n");
				}
				case 1: { // Quit
					quit = true;
					break;
				}
				case 2: { // Restart game
					change_room(first_room, false);
					break;
				}
				case 3: { // Restart room
					change_room(current_room, false);
					break;
				}
				case 4: { // Jump to loop end, e.g. change room
					break;
				}
			}
		}
	}

	current_room->room_end();
	current_room->game_end();
	is_ready = false;
	current_room->reset_properties();

	return 0;
}
int BEE::close() {
	if (is_initialized) {
		free_media();
		close_resources();
	}

	if (texture_before != NULL) {
		texture_before->free();
		delete texture_before;
		texture_before = NULL;
	}
	if (texture_after != NULL) {
		texture_after->free();
		delete texture_after;
		texture_after = NULL;
	}

	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
		renderer = NULL;
	}
	if (window != NULL) {
		SDL_DestroyWindow(window);
		window = NULL;
	}

	if (options->is_network_enabled) {
		if (!network_quit()) {
			net->is_initialized = false;
		}
	}

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	if (resource_list != NULL) {
		resource_list->reset();
		delete resource_list;
		resource_list = NULL;
	}

	return 0;
}

#ifndef _WINDOWS
#include "game/resources.cpp"
#endif // _WINDOWS

Uint32 BEE::get_ticks() {
	return SDL_GetTicks();
}
Uint32 BEE::get_seconds() {
	return SDL_GetTicks()/1000;
}
Uint32 BEE::get_frame() {
	return frame_number;
}

int BEE::render() {
	if (options->is_opengl) {
		// OpenGL rendering
	} else {
		SDL_RenderPresent(renderer);
	}
	return 0;
}
int BEE::render_clear() {
	if (options->is_opengl) {
		// OpenGL render clearing
	} else {
		SDL_RenderClear(renderer);
	}
	return 0;
}
int BEE::render_reset() {
	if (options->is_opengl) {
		// OpenGL render reset
	} else {
		SDL_DestroyRenderer(renderer);

		int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
		if (options->is_vsync_enabled) {
			renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
		}
		renderer = SDL_CreateRenderer(window, -1, renderer_flags);
		if (renderer == NULL) {
			throw std::string("Couldn't create SDL renderer: ") + SDL_GetError() + "\n";
		}
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

		for (int i=0; i<resource_list->sprites.get_amount(); i++) {
			if (get_sprite(i) != NULL) {
				Sprite* s = get_sprite(i);
				if (s->get_is_loaded()) {
					s->free();
					s->load();
				}
			}
		}
		for (int i=0; i<resource_list->backgrounds.get_amount(); i++) {
			if (get_background(i) != NULL) {
				Background* s = get_background(i);
				if (s->get_is_loaded()) {
					s->free();
					s->load();
				}
			}
		}
	}
	return 0;
}

int BEE::restart_game() {
	throw 2;
	return 0;
}
int BEE::end_game() {
	throw 1;
	return 0;
}

#ifndef _WINDOWS
#include "game/room.cpp"
#include "game/transition.cpp"
#include "game/display.cpp"
#include "game/window.cpp"
#include "game/input.cpp"
#include "game/draw.cpp"
#include "game/network.cpp"
#endif // _WINDOWS

#endif // _BEE_GAME
