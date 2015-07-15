#include <iostream>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Sprite; class Sound; class Background; class Path; class Object; class Room;

class EGE {
		int argc;
		char** argv;
		bool quit, restart;
		Room *first_room = NULL, *current_room = NULL;
		
		SDL_Event event;
		Uint32 tickstamp, new_tickstamp;
	public:
		SDL_Window* window = NULL;
		SDL_Renderer* renderer = NULL;
		int width, height;
		unsigned int fps;
		
		EGE(int, char**, Room*);
		int loop();
		int close();
		int set_engine_pointer();
		int load_media();
		int free_media();
		
		int restart_room();
		int restart_room_internal();
		int change_room(Room*);
		int animation_end(Sprite*);
};

int init_resources();
int close_resources();
#include "resource_structures.hpp"
#include "../resources/resources.hpp"

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

EGE::EGE(int new_argc, char** new_argv, Room* new_first_room) {
	argc = new_argc;
	argv = new_argv;
	fps = 60;
	
	width = DEFAULT_WINDOW_WIDTH;
	height = DEFAULT_WINDOW_HEIGHT;
	
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		throw std::string("Couldn't init SDL: ") + SDL_GetError() + "\n";
	}
	
	window = SDL_CreateWindow("Easy Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (window == NULL) {
		throw std::string("Couldn't create SDL window: ") + SDL_GetError() + "\n";
	}
	
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		throw std::string("Couldn't create SDL renderer: ") + SDL_GetError() + "\n";
	}
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	
	int img_flags = IMG_INIT_PNG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		throw std::string("Couldn't init SDL_image: ") + IMG_GetError() + "\n";
	}
	
	if ((!is_initialized)&&(init_resources())) {
		throw std::string("Couldn't init resources\n");
	}
	
	if (set_engine_pointer()) {
		throw std::string("Couldn't set engine pointer\n");
	}
	
	quit = false;
	restart = false;
	if (new_first_room != NULL) {
		if (change_room(new_first_room)) {
			throw std::string("Couldn't load first room\n");
		}
	}
}
int EGE::loop() {
	if (current_room == NULL) {
		quit = true;
		return 1;
	}
	
	tickstamp = SDL_GetTicks();
	while (!quit) {
		try {
			current_room->step_begin();
			current_room->alarm(0);
			
			while (SDL_PollEvent(&event) != 0) {
				switch (event.type) {
					case SDL_QUIT: {
						quit = true;
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
				}
			}
			
			current_room->step_mid();
			current_room->path_end();
			current_room->outside_room();
			current_room->intersect_boundary();
			current_room->collision();
			
			current_room->step_end();
			current_room->draw();
			
			new_tickstamp = SDL_GetTicks();
			if (new_tickstamp - tickstamp < 1000/fps) {
				SDL_Delay((1000/fps) - (new_tickstamp - tickstamp));
			}
			tickstamp = new_tickstamp;
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
	
	return 0;
}
int EGE::close() {
	free_media();
	close_resources();
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;
	
	IMG_Quit();
	SDL_Quit();
	
	return 0;
}
int EGE::set_engine_pointer() {
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
int EGE::load_media() {
	if (current_room != NULL) {
		current_room->load_media();
	}
	
	return 0;
}
int EGE::free_media() {
	if (current_room != NULL) {
		current_room->free_media();
	}
	
	return 0;
}

int EGE::restart_room() {
	restart = true;
	throw 3;
	return 0;
}
int EGE::restart_room_internal() {
	current_room->room_end();
	current_room->reset_properties();
	current_room->init();
	
	SDL_SetWindowTitle(window, current_room->get_name().c_str());
	
	current_room->create();
	current_room->room_start();
	current_room->draw();
	
	restart = false;
	
	return 0;
}
int EGE::change_room(Room* new_room) {
	if (quit) {
		return 1;
	}
	
	bool is_game_start = false;
	if (current_room != NULL) {
		current_room->room_end();
	} else {
		is_game_start = true;
		first_room = new_room;
	}
	
	free_media();
	
	current_room = new_room;
	current_room->reset_properties();
	current_room->init();
	
	if (load_media()) {
		std::cerr << "Couldn't load room media for " << current_room->get_name() << "\n";
		return 1;
	}
	
	SDL_SetWindowTitle(window, current_room->get_name().c_str());
	std::cout << current_room->get_instance_string();
	
	current_room->create();
	if (is_game_start) {
		current_room->game_start();
	}
	current_room->room_start();
	current_room->draw();
	
	return 0;
}
int EGE::animation_end(Sprite* finished_sprite) {
	return current_room->animation_end(finished_sprite);
}
