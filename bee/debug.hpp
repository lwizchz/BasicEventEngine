/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_DEBUG_H
#define _BEE_DEBUG_H 1

#include <string>
#include <sstream>
#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>

std::string debug_indent(std::string, int, std::string);
std::string debug_indent(std::string, int);
std::string get_shader_error(GLuint);
std::string get_program_error(GLuint);

#endif // _BEE_DEBUG_H
