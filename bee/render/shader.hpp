/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_SHADER_H
#define BEE_RENDER_SHADER_H 1

#include "../defines.hpp"

#include <string>
#include <map>
#include <vector>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>

namespace bee {
	struct ShaderInput {
		bool is_attrib;
		bool is_required;
		GLint location;

		ShaderInput();
		explicit ShaderInput(bool, bool);
	};

	class Shader {
			std::string filename;
			GLenum type;
			GLuint shader;
		public:
			Shader(const std::string&, GLenum);

			GLuint get_shader() const;

			int compile();
	};

	class ShaderProgram {
			GLuint program;
			std::vector<Shader> shaders;
			std::map<std::string,ShaderInput> inputs;
		public:
			ShaderProgram();

			int delete_shaders();

			int add_shader(Shader&);
			int add_attrib(const std::string&, bool);
			int add_uniform(const std::string&, bool);
			int link();

			GLuint get_program() const;
			GLint get_location(const std::string&) const;

			int apply();
	};
}

#endif // BEE_RENDER_SHADER_H
