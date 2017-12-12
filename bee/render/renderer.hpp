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
	class ShaderProgram;

	class Renderer {
		public:
			// These are used for the SDL renderer
			SDL_Window* window;
			SDL_Renderer* sdl_renderer;

			// This is the OpenGL renderer context
			SDL_GLContext context;

			ShaderProgram* program;

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
			int sdl_renderer_init();
			int sdl_renderer_close();

			int reset();
	};
}

#endif // BEE_RENDERER_H
