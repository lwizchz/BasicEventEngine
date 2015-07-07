#include <iostream>
#include <SDL2/SDL.h>

class EGE;
struct Game {
	SDL_Window* window;
	SDL_Surface* screen_surface;
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
	game.screen_surface = NULL;
	
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cerr << "Couldn't init SDL: " << SDL_GetError() << "\n";
		quit = true;
		return;
	}
	
	game.window = SDL_CreateWindow("Easy Game Engine", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (game.window == NULL) {
		std::cerr << "Couldn't create SDL window: " << SDL_GetError() << "\n";
		quit = true;
		return;
	}
	
	game.screen_surface = SDL_GetWindowSurface(game.window);
	SDL_FillRect(game.screen_surface, NULL, SDL_MapRGB(game.screen_surface->format, 0xFF, 0xFF, 0xFF));
	SDL_UpdateWindowSurface(game.window);
	
	quit = false;
	if (first_room != NULL) {
		change_room(first_room);
	}
}
int EGE::loop() {
	while (!quit) {
		current_room->step_begin();
		current_room->alarm(0);
		
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				quit = true;
			}
		}
		current_room->keyboard(event.type);
		current_room->mouse(event.type);
		current_room->keyboard_press(event.type);
		current_room->mouse_press(event.type);
		current_room->keyboard_release(event.type);
		current_room->mouse_release(event.type);
		
		current_room->step_mid();
		current_room->path_end();
		current_room->outside_room();
		current_room->intersect_boundary();
		current_room->collision();
		
		current_room->step_end();		
		current_room->draw();
		current_room->animation_end();
	}
	
	current_room->room_end();
	current_room->game_end();
	
	return 0;
}
int EGE::close() {
	free_media();
	
	SDL_DestroyWindow(game.window);
	game.window = NULL;
	
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
