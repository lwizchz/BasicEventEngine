/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_DEBUG_H
#define BEE_DEBUG_H 1

#include <string> // Include the required library headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

// Format indentions for debug output
std::string debug_indent(const std::string&, int, const std::string&);
std::string debug_indent(const std::string&, int);

// Return the error string when an OpenGL item fails to compile
std::string get_shader_error(GLuint);
std::string get_program_error(GLuint);

// Return the SDL error string as a C++ string
std::string get_sdl_error();

#endif // BEE_DEBUG_H
