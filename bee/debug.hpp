/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_DEBUG_H
#define _BEE_DEBUG_H 1

#include <string> // Include the required library headers
#include <sstream>

#define GLEW_STATIC // Statically link GLEW
#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>

// Format indentions for debug output
std::string debug_indent(const std::string&, int, const std::string&);
std::string debug_indent(const std::string&, int);

// Return the error string when an OpenGL item fails to compile
std::string get_shader_error(GLuint);
std::string get_program_error(GLuint);

#endif // _BEE_DEBUG_H
