/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDERER_H
#define BEE_RENDERER_H 1

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

#include "../defines.hpp"

namespace bee {
	class Camera;

	class Renderer {
		public:
			// These are used for the SDL renderer
			SDL_Window* window;
			SDL_Renderer* sdl_renderer;

			// This is the OpenGL renderer context
			SDL_GLContext context;

			// The following GLint's are all uniforms in the OpenGL shaders
			GLuint program = 0; // This is the location of the OpenGL program (where the shaders are compiled)
			GLint vertex_location = -1;
			GLint normal_location = -1;
			GLint fragment_location = -1;
			GLuint target = 0;

			GLint projection_location = -1;
			GLint view_location = -1;
			GLint model_location = -1;
			GLint port_location = -1;

			GLint rotation_location = -1;

			GLint texture_location = -1;
			GLint colorize_location = -1;
			GLint primitive_location = -1;
			GLint flip_location = -1;

			GLint is_lightable_location = -1;
			GLint light_amount_location = -1;
			struct {
				GLint type;
				GLint position;
				GLint direction;
				GLint attenuation;
				GLint color;
			} lighting_location[BEE_MAX_LIGHTS];

			GLint lightable_amount_location = -1;
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
