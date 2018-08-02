/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_TEXTURE_H
#define BEE_TEXTURE_H 1

#include "../defines.hpp"

#include <string> // Include the required library headers
#include <map>
#include <vector>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

#include "resource.hpp"

#include "../data/variant.hpp"

#include "../render/rgba.hpp"

namespace bee {
	/// Used to pass transform data to the Texture class
	struct TextureTransform {
		int x; ///< The top-left x-coordinate
		int y; ///< The top-left y-coordinate

		bool is_horizontal_tile; ///< Whether the Texture should be tiled horizontally
		bool is_vertical_tile; ///< Whether the Texture should be tiled vertically
		int horizontal_speed; ///< The speed with which the Texture should move horizontally in pixels per second
		int vertical_speed; ///< The speed with which the Texture should move vertically in pixels per second

		bool is_stretched; ///< Whether the Texture should be stretched to the window size, which will cause some of the above values to be ignored

		// See bee/resource/texture.cpp for function comments
		TextureTransform();
		TextureTransform(int, int, bool, bool, int, int, bool);
	};

	/// Used to pass Texture render data to the rendering system
	struct TextureDrawData {
		GLuint vao; ///< The Vertex Array %Object
		GLuint texture; ///< The OpenGL texture
		GLuint ibo; ///< The Index Buffer %Object

		glm::mat4 model; ///< The model transform
		glm::mat4 rotation; ///< The rotational transform
		glm::vec4 color; ///< The colorization to use
		GLuint buffer; ///< The texcoord buffer

		// See bee/resource/texture.cpp for function comments
		TextureDrawData(GLuint, GLuint, GLuint);
		TextureDrawData(GLuint, GLuint, GLuint, glm::mat4, glm::mat4, glm::vec4, GLuint);
	};

	/// Used to draw all 2D items
	class Texture: public Resource {
		static std::map<size_t,Texture*> list;
		static size_t next_id;

		size_t id; ///<  The unique Texture identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the image file

		unsigned int width; ///< The width of the image
		unsigned int height; ///< The height of the image
		unsigned int subimage_amount; ///< The number of subimages to divide the images into
		unsigned int subimage_width; ///< The width of each subimage

		SDL_Rect crop; ///< A rectangle which determines how the Texture is cropped before being drawn
		double speed; ///< The speed at which the subimages animate in frames per second
		bool is_animated; ///< Whether the Texture is currently animating or not
		std::pair<int,int> origin; ///< The origin from which the Texture is drawn
		std::pair<double,double> rotate; ///< The origin around which the Texture is rotated, scaled from 0.0 to 1.0 in both width and height

		bool is_loaded; ///< Whether the image file was successfully loaded into a texture
		SDL_Texture* texture; ///< The internal texture storage
		bool has_draw_failed; ///< Whether the draw function has previously failed, this prevents continuous warning outputs

		GLuint vao; ///< The Vertex Array %Object which contains most of the following data
		GLuint vbo_vertices; ///< The Vertex Buffer %Object which contains the vertices of the quad
		GLuint ibo; ///< The buffer object which contains the order of the vertices for each element
		GLuint gl_texture; ///< The internal texture storage for OpenGL mode
		std::vector<GLuint> vbo_texcoords; ///< The buffer objects which contains the subimage texture coordinates

		GLuint framebuffer; ///< The framebuffer object used by set_as_target()

		// See bee/resource/texture.cpp for function comments
		int drawing_begin();
		int drawing_end();

		int tile_horizontal(const SDL_Rect&);
		int tile_vertical(const SDL_Rect&);
	public:
		// See bee/resource/texture.cpp for function comments
		Texture();
		Texture(const std::string&, const std::string&);
		~Texture();

		static size_t get_amount();
		static Texture* get(size_t);
		static Texture* get_by_name(const std::string&);
		static Texture* add(const std::string&, const std::string&);

		size_t add_to_resources();
		int reset();

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&);
		void print() const;

		size_t get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		std::pair<int,int> get_size() const;
		int get_subimage_amount() const;
		int get_subimage_width() const;
		double get_speed() const;
		bool get_is_animated() const;
		std::pair<int,int> get_origin() const;
		std::pair<double,double> get_rotate() const;
		SDL_Texture* get_texture() const;
		bool get_is_loaded() const;

		void set_name(const std::string&);
		void set_path(const std::string&);
		void set_speed(double);
		void set_origin(int, int);
		void set_origin_center();
		void set_rotate(double, double);
		void set_subimage_amount(int, int);
		void crop_image(SDL_Rect);

		int load_from_surface(SDL_Surface*);
		SDL_Surface* load_surface() const;
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
