/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_TEXTURE_H
#define BEE_TEXTURE_H 1

#include "../defines.hpp"

#include <string> // Include the required library headers
#include <map>
#include <vector>
#include <list>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

#include "resource.hpp"

#include "../render/rgba.hpp"

namespace bee {
	struct TextureTransform { // The data struct which is used to pass transform data to the Texture class
		int x, y; // The coordinates of the desired draw location
		bool is_horizontal_tile, is_vertical_tile; // Whether the texture should be tiled horizontally and vertically
		int horizontal_speed, vertical_speed; // The speed with which the texture should move horizontally and vertically in pixels per second
		bool is_stretched; // Whether the texture should be stretched to the window size, note that stretched textures will not be animated or tiled

		// See bee/resources/texture.cpp for function comments
		TextureTransform();
		TextureTransform(int, int, bool, bool, int, int, bool);
	};

	struct TextureDrawData {
		GLuint vao;
		GLuint texture;
		GLuint ibo;

		glm::mat4 model;
		glm::mat4 rotation;
		glm::vec4 color;
		GLuint buffer;

		// See bee/resources/texture.cpp for function comments
		TextureDrawData(GLuint, GLuint, GLuint);
		TextureDrawData(GLuint, GLuint, GLuint, glm::mat4, glm::mat4, glm::vec4, GLuint);
	};

	class Texture: public Resource { // The texture resource class is used to draw all on-screen objects
			static std::map<int,Texture*> list;
			static int next_id;

			int id; // The id of the resource
			std::string name; // An arbitrary name for the resource
			std::string path; // The path of the image file which is used as the texture
			unsigned int width, height; // The width and height of the texture
			unsigned int subimage_amount, subimage_width; // The variables which determine how the subimages are divided
			SDL_Rect crop; // A rectangle which determines how the texture is cropped before being drawn
			double speed; // The speed at which the subimages animate
			bool is_animated; // Whether the texture is currently animating or not
			int origin_x, origin_y; // The origin from which the texture is drawn
			double rotate_x, rotate_y; // The origin around which the texture is rotated, scaled from 0.0 to 1.0 in both width and height

			SDL_Texture* texture; // The internal texture storage for SDL mode
			bool is_loaded; // Whether the image file was successfully loaded into a texture
			bool has_draw_failed; // Whether the draw function has previously failed, this prevents continuous warning outputs

			GLuint vao; // The Vertex Array Object which contains most of the following data
			GLuint vbo_vertices; // The Vertex Buffer Object which contains the vertices of the quad
			GLuint ibo; // The buffer object which contains the order of the vertices for each element
			GLuint gl_texture; // The internal texture storage for OpenGL mode
			std::vector<GLuint> vbo_texcoords; // The buffer object which contains the subimage texture coordinates

			GLuint framebuffer; // The framebuffer object used by set_as_target()

			// See bee/resources/texture.cpp for function comments
			int drawing_begin();
			int drawing_end();

			int tile_horizontal(const SDL_Rect&);
			int tile_vertical(const SDL_Rect&);
		public:
			// See bee/resources/texture.cpp for function comments
			Texture();
			Texture(const std::string&, const std::string&);
			~Texture();

			int add_to_resources();
			static size_t get_amount();
			static Texture* get(int);
			int reset();
			int print() const;

			int get_id() const;
			std::string get_name() const;
			std::string get_path() const;
			int get_width() const;
			int get_height() const;
			int get_subimage_amount() const;
			int get_subimage_width() const;
			double get_speed() const;
			bool get_is_animated() const;
			int get_origin_x() const;
			int get_origin_y() const;
			double get_rotate_x() const;
			double get_rotate_y() const;
			SDL_Texture* get_texture() const;
			bool get_is_loaded() const;

			int set_name(const std::string&);
			int set_path(const std::string&);
			int set_speed(double);
			int set_origin_xy(int, int);
			int set_origin_x(int);
			int set_origin_y(int);
			int set_origin_center();
			int set_rotate_xy(double, double);
			int set_rotate_x(double);
			int set_rotate_y(double);
			int set_rotate_center();
			int set_subimage_amount(int, int);
			int crop_image(SDL_Rect);
			int crop_image_width(int);
			int crop_image_height(int);

			int load_from_surface(SDL_Surface*);
			int load();
			int load_as_target(int, int);
			int free();

			int draw_subimage(int, int, unsigned int, int, int, double, RGBA);
			int draw(int, int, Uint32, int, int, double, RGBA);
			int draw(int, int, Uint32);
			int draw_transform(const TextureTransform&);
			GLuint set_as_target();
	};
}

#endif // BEE_TEXTURE_H
