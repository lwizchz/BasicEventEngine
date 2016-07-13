/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SPRITE_H
#define _BEE_SPRITE_H 1

#include <string>
#include <vector>
#include <list>
#include <SDL2/SDL.h>

#include "../game.hpp"

class BEE::SpriteDrawData {
	public:
		int x = 0, y = 0;
		Uint32 subimage_time = 0;
		int w = 0, h = 0;
		double angle = 0.0;
};

class BEE::Sprite: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string image_path;
		int width, height;
		int subimage_amount, subimage_width;
		SDL_Rect crop;
		double speed, alpha;
		bool is_animated;
		int origin_x, origin_y;
		double rotate_x, rotate_y;

		SDL_Texture* texture;
		bool is_loaded = false;
		bool has_draw_failed = false;
		std::vector<SDL_Rect> subimages;
		SDL_Rect srect, drect;

		GLuint vbo_vertices;
		GLuint ibo;
		GLuint gl_texture;
		std::vector<GLuint> vbo_texcoords;
		bool is_lightable = true;

		GLuint framebuffer;
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
		double get_rotate_x();
		double get_rotate_y();
		SDL_Texture* get_texture();
		bool get_is_loaded();
		bool get_is_lightable();

		int set_name(std::string);
		int set_path(std::string);
		int set_subimage_amount(int, int);
		int crop_image(SDL_Rect);
		int crop_image_width(int);
		int crop_image_height(int);
		int set_speed(double);
		int set_alpha(double);
		int set_origin_x(int);
		int set_origin_y(int);
		int set_origin_xy(int, int);
		int set_origin_center();
		int set_rotate_x(double);
		int set_rotate_y(double);
		int set_rotate_xy(double, double);
		int set_is_lightable(bool);

		int load_from_surface(SDL_Surface*);
		int load();
		int free();
		int draw_subimage(int, int, int, int, int, double, RGBA, SDL_RendererFlip, bool);
		int draw(int, int, Uint32, int, int, double, RGBA, SDL_RendererFlip, bool);
		int draw(int, int, Uint32);
		int draw(int, int, Uint32, int, int);
		int draw(int, int, Uint32, double);
		int draw(int, int, Uint32, RGBA);
		int draw(int, int, Uint32, SDL_RendererFlip);
		int draw(int, int, Uint32, bool);
		int draw_simple(SDL_Rect*, SDL_Rect*);
		int draw_array(const std::list<SpriteDrawData*>&, const std::vector<glm::mat4>&, RGBA, SDL_RendererFlip, bool);
		int set_as_target(int, int);
		int set_as_target();
};

#endif // _BEE_SPRITE_H
