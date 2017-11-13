/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDERER_H
#define BEE_RENDERER_H 1

#include "../defines.hpp"

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

namespace bee {
	// Forward declaration
	struct Camera;

	class Renderer {
		public:
			// These are used for the SDL renderer
			SDL_Window* window;
			SDL_Renderer* sdl_renderer;

			// This is the OpenGL renderer context
			SDL_GLContext context;

			// The following GLint's are all uniforms in the OpenGL shaders
			GLuint program; // This is the location of the OpenGL program (where the shaders are compiled)
			GLint vertex_location;
			GLint normal_location;
			GLint fragment_location;
			GLuint target;

			GLint projection_location;
			GLint view_location;
			GLint model_location;
			GLint port_location;

			GLint rotation_location;

			GLint texture_location;
			GLint colorize_location;
			GLint primitive_location;
			GLint flip_location;
			GLint time_location;

			GLint is_lightable_location;
			GLint light_amount_location;
			struct {
				GLint type;
				GLint position;
				GLint direction;
				GLint attenuation;
				GLint color;
			} lighting_location[BEE_MAX_LIGHTS];

			GLint lightable_amount_location;
			struct {
				GLint position;
				GLint vertex_amount;
				GLint mask[BEE_MAX_MASK_VERTICES];
			} lightable_location[BEE_MAX_LIGHTABLES];

			bool render_is_3d;
			Camera* render_camera;
			glm::mat4* projection_cache;

			// These should only be used internally by the functions in bee/render/drawing.cpp
			GLuint triangle_vao;
			GLuint triangle_vbo;
			GLuint triangle_ibo;

			Renderer();
			~Renderer();

			int opengl_init();
			int opengl_close();
			std::string opengl_prepend_version(const std::string&);
			int sdl_renderer_init();
			int sdl_renderer_close();

			int render_clear();
			int render() const;
			int render_reset();
	};
}

#endif // BEE_RENDERER_H
