/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SPRITE_H
#define _BEE_SPRITE_H 1

#include <string>
#include <vector>
#include <SDL2/SDL.h>

#include "../game.hpp"

class BEE::Sprite: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string image_path;
		int width, height;
		int subimage_amount, subimage_width;
		double speed, alpha;
		bool is_animated;
		int origin_x, origin_y;

		SDL_Texture* texture;
		bool is_loaded = false;
		std::vector<SDL_Rect> subimages;
		SDL_Rect srect, drect;
	public:
		Sprite();
		Sprite(std::string, std::string);
		~Sprite();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		int get_width();
		int get_height();
		int get_subimage_amount();
		int get_subimage_width();
		double get_speed();
		double get_alpha();
		bool get_is_animated();
		int get_origin_x();
		int get_origin_y();
		SDL_Texture* get_texture();
		bool get_is_loaded();

		int set_name(std::string);
		int set_path(std::string);
		int set_subimage_amount(int, int);
		int set_speed(double);
		int set_alpha(double);
		int set_origin_x(int);
		int set_origin_y(int);
		int set_origin_xy(int, int);
		int set_origin_center();

		int load();
		int free();
		int draw(int, int, Uint32, int, int, double, RGBA);
		int draw(int, int, Uint32);
		int draw(int, int, Uint32, int, int);
		int draw(int, int, Uint32, double);
		int draw(int, int, Uint32, RGBA);
		int set_as_target();
};

#endif // _BEE_SPRITE_H
