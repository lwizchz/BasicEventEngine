/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_BACKGROUND_H
#define _BEE_BACKGROUND_H 1

#include "../game.hpp"

class BEE::Background: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string background_path;
		int width, height;
		bool is_tiling;
		int tile_width, tile_height;
		Uint32 animation_time;

		SDL_Texture* texture;
		bool is_loaded = false;

		int tile_horizontal(SDL_Texture*, SDL_Rect*);
		int tile_vertical(SDL_Texture*, SDL_Rect*);
	public:
		Background();
		Background(std::string, std::string);
		~Background();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		bool get_is_tiling();
		int get_tile_width();
		int get_tile_height();

		int set_name(std::string);
		int set_path(std::string);
		int set_is_tiling(bool);
		int set_tile_width(int);
		int set_tile_height(int);
		int set_time_update();

		int load();
		int free();
		int draw(int, int, BackgroundData*);
};

#endif // _BEE_BACKGROUND_H
