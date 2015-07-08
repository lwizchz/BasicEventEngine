#include <iostream>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class EGE;
struct Game {
	SDL_Window* window;
	SDL_Renderer* renderer;
	int width, height;
	unsigned int fps;
	EGE* g;
} game;

#include "resource_structures.hpp"
#include "../resources/resources.hpp"

#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720

class EGE {
		int argc;
		char** argv;
		bool quit;
		Room* current_room;
		
		SDL_Event event;
		Uint32 tickstamp, new_tickstamp;
	public:
		EGE(int, char**, Room*);
		int loop();
		int close();
		int load_media();
		int free_media();
		
		int change_room(Room*);
};

EGE::EGE(int new_argc, char** new_argv, Room* first_room) {
	argc = new_argc;
	argv = new_argv;
	game.window = NULL;
	game.renderer = NULL;
	game.fps = 60;
	
	game.width = DEFAULT_WINDOW_WIDTH;
	game.height = DEFAULT_WINDOW_HEIGHT;
	
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cerr << "Couldn't init SDL: " << SDL_GetError() << "\n";
		quit = true;
		return;
	}
	
	game.window = SDL_CreateWindow("Easy Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, game.width, game.height, SDL_WINDOW_SHOWN);
	if (game.window == NULL) {
		std::cerr << "Couldn't create SDL window: " << SDL_GetError() << "\n";
		quit = true;
		return;
	}
	
	game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);
	if (game.renderer == NULL) {
		std::cerr << "Couldn't create SDL renderer: " << SDL_GetError() << "\n";
		quit = true;
		return;
	}
	SDL_SetRenderDrawColor(game.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	
	int img_flags = IMG_INIT_PNG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		std::cerr << "Couldn't init SDL_image: " << IMG_GetError() << "\n";
		quit = true;
		return;
	}
	
	quit = false;
	if (first_room != NULL) {
		change_room(first_room);
	}
}
int EGE::loop() {
	tickstamp = SDL_GetTicks();
	while (!quit) {
		current_room->step_begin(); if (quit) {break;}
		current_room->alarm(0); if (quit) {break;}
		
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
			if (quit) {break;}
		}
		if (quit) {break;}
		
		current_room->step_mid(); if (quit) {break;}
		current_room->path_end(); if (quit) {break;}
		current_room->outside_room(); if (quit) {break;}
		current_room->intersect_boundary(); if (quit) {break;}
		current_room->collision(); if (quit) {break;}
		
		current_room->step_end(); if (quit) {break;}
		current_room->draw(); if (quit) {break;}
		current_room->animation_end(); if (quit) {break;}
		
		new_tickstamp = SDL_GetTicks();
		if (new_tickstamp - tickstamp < 1000/game.fps) {
			SDL_Delay((1000/game.fps) - (new_tickstamp - tickstamp));
		}
		tickstamp = new_tickstamp;
	}
	
	current_room->room_end();
	current_room->game_end();
	
	return 0;
}
int EGE::close() {
	free_media();
	
	SDL_DestroyRenderer(game.renderer);
	SDL_DestroyWindow(game.window);
	game.window = NULL;
	game.renderer = NULL;
	
	IMG_Quit();
	SDL_Quit();
	
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

int EGE::change_room(Room* new_room) {
	if (quit) {
		return 1;
	}
	
	bool is_game_start = false;
	if (current_room != NULL) {
		current_room->room_end();
	} else {
		is_game_start = true;
	}
	
	free_media();
	current_room = new_room;
	if (load_media()) {
		std::cerr << "Couldn't load room media for " << current_room->get_name() << "\n";
		return 1;
	}
	
	current_room->create();
	if (is_game_start) {
		current_room->game_start();
	}
	current_room->room_start();
	current_room->draw();
	
	return 0;
}
