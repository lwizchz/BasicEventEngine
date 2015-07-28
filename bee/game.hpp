/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_H
#define _BEE_GAME_H 1

#include <iostream>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "util.hpp"

class Sprite; class Sound; class Background; class Path; class Object; class Room;

class GameOptions {
	public:
		// Window flags
		bool is_fullscreen, is_opengl, is_borderless;
		bool is_resizable, is_maximized, is_highdpi;

		// Renderer flags
		bool is_vsync_enabled;
};

class BEE {
		int argc;
		char** argv;
		bool quit, is_ready;
		Room *first_room = NULL, *current_room = NULL;
		GameOptions* options;

		bool is_minimized, is_fullscreen;
		bool has_mouse, has_focus;

		SDL_Event event;
		Uint32 tickstamp, new_tickstamp, fps_ticks;
	public:
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;
		int width, height;
		unsigned int fps_max, fps_goal, fps_count, fps_stable;

		BEE(int, char**, Room*, GameOptions*);
		~BEE();
		int loop();
		int close();
		int set_engine_pointer();
		int load_media();
		int free_media();

		int render();
		int render_clear();

		int restart_game();
		int end_game();

		int restart_room();
		int restart_room_internal();
		int change_room(Room*);
		int animation_end(Sprite*);
		static void sound_finished(int);

		int set_render_target(Sprite*);

		Room* get_room();
		bool get_is_ready();
};

int init_resources();
int close_resources();
#include "resource_structures.hpp"
#include "../resources/resources.hpp"

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

BEE::BEE(int new_argc, char** new_argv, Room* new_first_room, GameOptions* new_options) {
	argc = new_argc;
	argv = new_argv;
	is_ready = false;
	options = new_options;

	is_minimized = false;
	is_fullscreen = false;
	has_mouse = false;
	has_focus = false;

	fps_max = 300;
	fps_goal = 60;
	fps_count = 0;
	fps_stable = 0;

	width = DEFAULT_WINDOW_WIDTH;
	height = DEFAULT_WINDOW_HEIGHT;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) > 0) {
		throw std::string("Couldn't init SDL: ") + SDL_GetError() + "\n";
	}

	int window_flags = SDL_WINDOW_SHOWN;
	if (options->is_fullscreen) {
		if (options->is_resizable) {
			window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP; // Changes the window dimensions
		} else {
			window_flags |= SDL_WINDOW_FULLSCREEN; // Changes the video mode
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
	window = SDL_CreateWindow("Easy Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, window_flags);
	if (window == NULL) {
		throw std::string("Couldn't create SDL window: ") + SDL_GetError() + "\n";
	}

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

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		throw std::string("Couldn't init SDL_mixer: ") + Mix_GetError() + "\n";
	}
	Mix_ChannelFinished(sound_finished);
	Mix_AllocateChannels(128); // Probably overkill

	if ((!is_initialized)&&(init_resources())) {
		throw std::string("Couldn't init resources\n");
	}

	if (set_engine_pointer()) {
		throw std::string("Couldn't set engine pointer\n");
	}

	quit = false;
	if (new_first_room != NULL) {
		if (change_room(new_first_room)) {
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

	tickstamp = SDL_GetTicks();
	fps_ticks = SDL_GetTicks();
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
							case SDL_WINDOWEVENT_RESIZED:
							case SDL_WINDOWEVENT_SIZE_CHANGED: {
								width = event.window.data1;
								height = event.window.data2;
								render();
								break;
							}
							case SDL_WINDOWEVENT_EXPOSED: {
								render();
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
							case SDL_WINDOWEVENT_MINIMIZED: {
								is_minimized = true;
								has_mouse = false;
								break;
							}case SDL_WINDOWEVENT_MAXIMIZED: {
								is_minimized = false;
								break;
							}case SDL_WINDOWEVENT_RESTORED: {
								is_minimized = false;
								break;
							}
						}
						current_room->window(&event);
						break;
					}

					case SDL_KEYDOWN: {
						if (event.key.repeat != 0) {
							current_room->keyboard(&event);
						} else {
							current_room->keyboard_press(&event);
						}
						break;
					}
					case SDL_KEYUP: {
						current_room->keyboard_release(&event);
						break;
					}
					case SDL_MOUSEMOTION:
					case SDL_MOUSEWHEEL: {
						current_room->mouse(&event);
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

			current_room->step_mid();
			current_room->path_end();
			current_room->outside_room();
			current_room->intersect_boundary();
			current_room->collision();

			current_room->step_end();
			current_room->draw();

			fps_count++;
			new_tickstamp = SDL_GetTicks();
			if ((new_tickstamp - tickstamp < 1000/fps_max)&&(!options->is_vsync_enabled)) {
				SDL_Delay((1000/fps_max) - (new_tickstamp - tickstamp));
			}
			tickstamp = SDL_GetTicks();

			if (tickstamp - fps_ticks >= 1000) {
				fps_stable = fps_count;
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
					if (current_room == first_room) {
						restart_room_internal();
					} else {
						change_room(first_room);
					}
					break;
				}
				case 3: { // Restart room
					restart_room_internal();
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
	free_media();
	close_resources();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	return 0;
}
int BEE::set_engine_pointer() {
	for (int i=0; i<resource_list.sprites.get_amount(); i++) {
		if (sprite(i) != NULL) {
			sprite(i)->game = this;
		}
	}
	for (int i=0; i<resource_list.sounds.get_amount(); i++) {
		if (sound(i) != NULL) {
			sound(i)->game = this;
		}
	}
	for (int i=0; i<resource_list.backgrounds.get_amount(); i++) {
		if (background(i) != NULL) {
			background(i)->game = this;
		}
	}
	for (int i=0; i<resource_list.fonts.get_amount(); i++) {
		if (font(i) != NULL) {
			font(i)->game = this;
		}
	}
	for (int i=0; i<resource_list.paths.get_amount(); i++) {
		if (path(i) != NULL) {
			path(i)->game = this;
		}
	}
	for (int i=0; i<resource_list.objects.get_amount(); i++) {
		if (object(i) != NULL) {
			object(i)->game = this;
		}
	}
	for (int i=0; i<resource_list.rooms.get_amount(); i++) {
		if  (room(i) != NULL) {
			room(i)->game = this;
		}
	}

	return 0;
}
int BEE::load_media() {
	if (current_room != NULL) {
		current_room->load_media();
	}

	return 0;
}
int BEE::free_media() {
	if (current_room != NULL) {
		current_room->free_media();
	}

	return 0;
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

int BEE::restart_game() {
	throw 2;
	return 0;
}
int BEE::end_game() {
	throw 1;
	return 0;
}

int BEE::restart_room() {
	throw 3;
	return 0;
}
int BEE::restart_room_internal() {
	current_room->room_end();
	is_ready = false;
	current_room->reset_properties();
	current_room->init();

	SDL_SetWindowTitle(window, current_room->get_name().c_str());

	is_ready = true;
	current_room->create();
	current_room->room_start();
	current_room->draw();

	return 0;
}
int BEE::change_room(Room* new_room) {
	if (quit) {
		return 1;
	}

	bool is_game_start = false;
	if (current_room != NULL) {
		current_room->room_end();
		current_room->reset_properties();
	} else {
		is_game_start = true;
		first_room = new_room;
	}

	free_media();

	current_room = new_room;
	is_ready = false;
	current_room->reset_properties();
	current_room->init();

	if (load_media()) {
		std::cerr << "Couldn't load room media for " << current_room->get_name() << "\n";
		return 1;
	}

	SDL_SetWindowTitle(window, current_room->get_name().c_str());
	std::cout << current_room->get_instance_string();

	is_ready = true;
	current_room->create();
	if (is_game_start) {
		current_room->game_start();
	}
	current_room->room_start();
	current_room->draw();

	return 0;
}

int BEE::animation_end(Sprite* finished_sprite) {
	return current_room->animation_end(finished_sprite);
}
void BEE::sound_finished(int channel) {
	for (int i=0; i<resource_list.sounds.get_amount(); i++) {
		if (sound(i) != NULL) {
			if (!sound(i)->get_is_music()) {
				sound(i)->finished(channel);
			}
		}
	}
}

int BEE::set_render_target(Sprite* sprite_target) {
	if (sprite_target == NULL) {
		SDL_SetRenderTarget(renderer, NULL);
	} else {
		sprite_target->set_as_target();
	}

	return 0;
}

Room* BEE::get_room() {
	return current_room;
}
bool BEE::get_is_ready() {
	return is_ready;
}

#endif // _BEE_GAME_H
