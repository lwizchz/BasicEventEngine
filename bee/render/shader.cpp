/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_SHADER
#define BEE_RENDER_SHADER 1

#include <SDL2/SDL.h> // Include the required SDL headers

#include "shader.hpp" // Include the engine headers

#include "../util/files.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"

#include "../messenger/messenger.hpp"

#include "render.hpp"

namespace bee {
	ShaderInput::ShaderInput() :
		is_attrib(false),
		is_required(true),
		location(-1)
	{}
	ShaderInput::ShaderInput(bool _is_attrib, bool _is_required) :
		is_attrib(_is_attrib),
		is_required(_is_required),
		location(-1)
	{}

	Shader::Shader(const std::string& _filename, GLenum _type) :
		filename(_filename),
		type(_type),
		shader(-1)
	{}

	GLuint Shader::get_shader() const {
		return shader;
	}

	int Shader::compile() {
		shader = glCreateShader(type);
		std::string sh = util::file_get_contents(filename);
		sh = render::opengl_prepend_version(sh);
		const GLchar* shader_source[] = {sh.c_str()};
		glShaderSource(shader, 1, shader_source, nullptr);

		glCompileShader(shader);

		GLint is_shader_compiled = GL_FALSE;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &is_shader_compiled);
		if (is_shader_compiled != GL_TRUE) {
			messenger::send({"engine", "renderer"}, E_MESSAGE::ERROR,
				"Couldn't compile OpenGL shader: " + std::to_string(shader) + "\n"
				+ util::get_shader_error(shader)
			);
			glDeleteShader(shader);
			shader = -1;
			return 1; // Return 1 when the shader could not be compiled
		}

		return 0;
	}

	ShaderProgram::ShaderProgram() :
		program(glCreateProgram()),
		shaders(),
		inputs()
	{}

	int ShaderProgram::delete_shaders() {
		for (auto& shader : shaders) {
			if (shader.get_shader() != static_cast<GLuint>(-1)) {
				glDeleteShader(shader.get_shader());
			}
		}
		return 0;
	}

	int ShaderProgram::add_shader(Shader& shader) {
		if (shader.compile() != 0) {
			return 1;
		}

		glAttachShader(program, shader.get_shader());

		shaders.push_back(shader);

		return 0;
	}
	int ShaderProgram::add_attrib(const std::string& name, bool is_required) {
		inputs.emplace(name, ShaderInput(true, is_required));
		return 0;
	}
	int ShaderProgram::add_uniform(const std::string& name, bool is_required) {
		inputs.emplace(name, ShaderInput(false, is_required));
		return 0;
	}
	int ShaderProgram::link() {
		glLinkProgram(program);

		GLint is_program_linked = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &is_program_linked);
		if (is_program_linked != GL_TRUE) {
			delete_shaders();

			messenger::send({"engine", "renderer"}, E_MESSAGE::ERROR,
				"Couldn't link OpenGL program: " + std::to_string(program) + "\n"
				+ util::get_program_error(program)
			);

			return 1; // Return 1 when the program could not be linked
		}

		// Locate inputs
		std::unordered_map<std::string,ShaderInput> _inputs (inputs);
		inputs.clear();
		for (auto& input : _inputs) {
			if (input.second.is_attrib) {
				input.second.location = glGetAttribLocation(program, input.first.c_str());
			} else {
				input.second.location = glGetUniformLocation(program, input.first.c_str());
			}

			if ((input.second.location == -1)&&(input.second.is_required)) {
				messenger::send({"engine", "renderer"}, E_MESSAGE::ERROR, "Couldn't get the location of \"" + input.first + "\" in the shader program");
				delete_shaders();
				return 2;
			}

			if (input.second.location != -1) {
				inputs.emplace(input.first, input.second);
			}
		}

		delete_shaders();

		return 0;
	}

	GLuint ShaderProgram::get_program() const {
		return program;
	}
	GLint ShaderProgram::get_location(const std::string& _input, bool should_print) const {
		std::unordered_map<std::string,ShaderInput>::const_iterator input (inputs.find(_input));
		if (input == inputs.end()) {
			if (should_print) {
				messenger::send({"engine", "renderer"}, E_MESSAGE::WARNING, "Shader input \"" + _input + "\" not found");
			}
			return -1;
		}

		return input->second.location;
	}
	GLint ShaderProgram::get_location(const std::string& input) const {
		return get_location(input, true);
	}

	int ShaderProgram::apply() {
		if (program == static_cast<GLuint>(-1)) {
			return 1;
		}

		render::set_program(this);
		glUseProgram(program);

		return 0;
	}
}

#endif // BEE_RENDER_SHADER
