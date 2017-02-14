/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SPRITE_H
#define _BEE_SPRITE_H 1

#include <string> // Include the required library headers
#include <vector>
#include <list>
#include <SDL2/SDL.h>

#include "../game.hpp" // Include the engine headers

class BEE::SpriteDrawData { // The data storage struct which is used in instanced drawing
	public:
		int x = 0, y = 0; // The coordinates of the desired draw location
		Uint32 subimage_time = 0; // The timestamp of the subimage animation
		int w = 0, h = 0; // The desired width and height of the sprite
		double angle = 0.0; // The desired rotation angle of the sprite
};

class BEE::Sprite: public Resource { // The sprite resource class with which all onscreen objects are drawn
		int id = -1; // The id of the resource
		std::string name; // An arbitrary name for the resource
		std::string path; // The path of the image file which is used as the sprite's texture
		unsigned int width, height; // The width and height of the texture
		unsigned int subimage_amount, subimage_width; // The variables which determine how the subimages are divided
		SDL_Rect crop; // A rectangle which determines how the sprite is cropped before being drawn
		double speed; // The speed at which the subimages animate
		double alpha; // The opacity of the texture from 0.0 to 1.0
		bool is_animated; // Whether the sprite is currently animating or not
		int origin_x, origin_y; // The origin from which the texture is drawn
		double rotate_x, rotate_y; // The origin around which the texture is rotated, scaled from 0.0 to 1.0 in both width and height

		SDL_Texture* texture; // The internal texture storage for SDL mode
		bool is_loaded = false; // Whether the image file was successfully loaded into a texture
		bool has_draw_failed = false; // Whether the draw function has previously failed, this prevents continuous writes to std::cerr
		std::vector<SDL_Rect> subimages; // A list of subimage coordinates and dimensions

		GLuint vao; // The Vertex Array Object which contains most of the following data
		GLuint vbo_vertices; // The Vertex Buffer Object which contains the vertices of the quad
		GLuint ibo; // The buffer object which contains the order of the vertices for each element
		GLuint gl_texture; // The internal texture storage for OpenGL mode
		std::vector<GLuint> vbo_texcoords; // The buffer object which contains the subimage texture coordinates
		bool is_lightable = true; // Whether the sprite is affected by the lighting system

		GLuint framebuffer; // The framebuffer object used by set_as_target()
	public:
		// See bee/resources/sprite.cpp for function comments
		Sprite();
		Sprite(std::string, std::string);
		~Sprite();
		int add_to_resources();
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
		int set_rotate_center();
		int set_is_lightable(bool);

		int load_from_surface(SDL_Surface*);
		int load();
		int free();
		int draw_subimage(int, int, unsigned int, int, int, double, RGBA, SDL_RendererFlip);
		int draw(int, int, Uint32, int, int, double, RGBA, SDL_RendererFlip);
		int draw(int, int, Uint32);
		int draw(int, int, Uint32, int, int);
		int draw(int, int, Uint32, double);
		int draw(int, int, Uint32, RGBA);
		int draw(int, int, Uint32, SDL_RendererFlip);
		int draw_simple(SDL_Rect*, SDL_Rect*);
		int draw_array(const std::list<SpriteDrawData*>&, const std::vector<glm::mat4>&, RGBA, SDL_RendererFlip);
		int set_as_target(int, int);
		int set_as_target();
};

#endif // _BEE_SPRITE_H
