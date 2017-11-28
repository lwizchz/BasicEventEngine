/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_BACKGROUND_H
#define BEE_BACKGROUND_H 1

#include <string> // Include the required library headers
#include <map>

#include "resource.hpp"

namespace bee {
	// Forward declaration
	class Texture;

	struct BackgroundData { // The data struct which is used to pass data to the Room class in bee/resources/room.hpp
		Texture* background; // A pointer to the background to use this data with
		bool is_visible; // Whether to draw the background
		bool is_foreground; // Whether to draw the texture above or below the other sprites
		int x, y; // The coordinates of the desired draw location
		bool is_horizontal_tile, is_vertical_tile; // Whether the texture should be tiled horizontally and vertically
		int horizontal_speed, vertical_speed; // The speed with which the texture should move horizontally and vertically in pixels per second
		bool is_stretched; // Whether the texture should be stretched to the window size, note that stretched textures will not be animated or tiled

		// See bee/resources/background.cpp for function comments
		BackgroundData();
		BackgroundData(Texture*, bool, bool, int, int, bool, bool, int, int, bool);
	};

	class Background: public Resource { // The background class is used to draw tiled textures behind or in front of all other on-screen objects
			static std::map<int,Background*> list;
			static int next_id;

			int id; // The id of the resource
			std::string name; // An arbitrary name for the resource
			std::string path; // The path of the image file which is used as the background's texture
			unsigned int width, height; // The width and height of the texture
			Uint32 animation_time; // The starting time of a movement animation for the texture

			SDL_Texture* texture; // The internal texture storage for SDL mode
			bool is_loaded; // Whether the image file was successfully loaded into a texture
			bool has_draw_failed; // Whether the draw function has previously failed, this prevents continous warning outputs

			GLuint vao; // The Vertex Array Object which contains most of the following data
			GLuint vbo_vertices; // The Vertex Buffer Object which contains the vertices of the quad
			GLuint ibo; // The buffer object which contains the order of the vertices for each element
			GLuint gl_texture; // The internal texture storage for OpenGL mode
			GLuint vbo_texcoords; // The buffer object which contains the subimage texture coordinates

			GLuint framebuffer; // The framebuffer object used by set_as_target()

			// See bee/resources/background.cpp for function comments
			int draw_internal(const SDL_Rect*, const SDL_Rect*, const glm::mat4&) const;
			int draw_internal(const SDL_Rect*, const SDL_Rect*) const;
			int tile_horizontal(const SDL_Rect*) const;
			int tile_vertical(const SDL_Rect*) const;

			int drawing_begin();
			int drawing_end();
		public:
			// See bee/resources/background.cpp for function comments
			Background();
			Background(const std::string&, const std::string&);
			~Background();

			int add_to_resources();
			static size_t get_amount();
			static Background* get(int);
			int reset();
			int print() const;

			int get_id() const;
			std::string get_name() const;
			std::string get_path() const;
			int get_width() const;
			int get_height() const;
			bool get_is_tiling() const;
			int get_tile_width() const;
			int get_tile_height() const;
			bool get_is_loaded() const;
			SDL_Texture* get_texture() const;

			int set_name(const std::string&);
			int set_path(const std::string&);
			int set_is_tiling(bool);
			int set_tile_width(int);
			int set_tile_height(int);
			int set_time_update();

			int load_from_surface(SDL_Surface*);
			int load();
			int free();
			int draw(int, int, BackgroundData*);
			int set_as_target(int, int);
			int set_as_target();
	};
}

#endif // BEE_BACKGROUND_H
