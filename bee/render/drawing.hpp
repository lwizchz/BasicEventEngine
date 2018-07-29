/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_DRAWING_H
#define BEE_RENDER_DRAWING_H 1

#include <string>

#include <SDL2/SDL.h> // Include the required SDL headers

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp> // Include the required OpenGL headers

#include "../enum.hpp"

#include "rgba.hpp"

namespace bee {
	int draw_triangle(glm::vec3, glm::vec3, glm::vec3, const RGBA&, bool);
	int draw_line(glm::vec3, glm::vec3, const RGBA&);
	int draw_line(int, int, int, int, const RGBA&);
	int draw_quad(glm::vec3, glm::vec3, int, const RGBA&);
	int draw_rectangle(int, int, int, int, int, const RGBA&);
	int draw_polygon(glm::vec3, double, double, double, unsigned int, int, const RGBA&);
	int draw_arc(glm::vec3, double, double, double, int, const RGBA&);
	int draw_circle(glm::vec3, double, int, const RGBA&);

	int draw_set_color(const RGBA&);
	RGBA draw_get_color();
	int draw_set_blend(GLenum, GLenum);
	std::pair<GLenum,GLenum> draw_get_blend();

	RGBA get_pixel_color(int, int);
	int save_screenshot(const std::string&);
}

#endif // BEE_RENDER_DRAWING_H
