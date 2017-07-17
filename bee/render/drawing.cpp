/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_RENDER_DRAWING
#define BEE_RENDER_DRAWING 1

#include "../defines.hpp"

#include <time.h> // Required for Windows time()

#include <GL/glew.h> // Include the required OpenGL headers
#include <SDL2/SDL_opengl.h>
#include <glm/gtc/type_ptr.hpp>

#include "drawing.hpp" // Include the engine headers

#include "../util/files.hpp"
#include "../util/platform.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"

#include "renderer.hpp"
#include "rgba.hpp"

namespace bee {
	/*
	* get_enum_color() - Return the RGBA value of the given E_RGB enumeration
	* @c: the E_RGB enumeration to convert
	* @a: the alpha value to use
	*/
	RGBA get_enum_color(E_RGB c, Uint8 a) {
		// Return a RGBA value with the given alpha value
		switch (c) {
			//case E_RGB::CYAN:
			case E_RGB::AQUA:           return {0, 255, 255, a};
			case E_RGB::BLUE:           return {0, 0, 255, a};
			case E_RGB::DKGRAY:         return {64, 64, 64, a};
			//case E_RGB::MAGENTA:
			case E_RGB::FUCHSIA:        return {255, 0, 255, a};
			case E_RGB::GRAY:           return {128, 128, 128, a};
			case E_RGB::GREEN:          return {0, 128, 0, a}; // Even though green is technically g=255, that color is called lime because it is quite bright
			case E_RGB::LIME:           return {0, 255, 0, a};
			//case E_RGB::LTGRAY:
			case E_RGB::SILVER:         return {192, 192, 192, a};
			case E_RGB::MAROON:         return {128, 0, 0, a};
			case E_RGB::NAVY:           return {0, 0, 128, a};
			case E_RGB::OLIVE:          return {128, 128, 0, a};
			case E_RGB::ORANGE:         return {255, 128, 0, a};
			case E_RGB::PURPLE:         return {128, 0, 255, a};
			case E_RGB::RED:            return {255, 0, 0, a};
			case E_RGB::TEAL:           return {0, 128, 128, a};
			case E_RGB::WHITE:          return {255, 255, 255, a};
			case E_RGB::YELLOW:         return {255, 255, 0, a};
			case E_RGB::BLACK: default: return {0, 0, 0, a}; // Return black if the enumeration is unknown
		}
	}
	/*
	* get_enum_color() - Return the RGBA value of the given E_RGB enumeration
	* ! When the function is called without an alpha value, simply call it with full opacity
	* @c: the E_RGB enumeration to convert
	*/
	RGBA get_enum_color(E_RGB c) {
		return get_enum_color(c, 255);
	}

	/*
	* draw_triangle() - Draw a triangle between the given vertices in the given color
	* @v1: the first vertex of the triangle
	* @v2: the second vertex of the triangle
	* @v3: the third vertex of the triangle
	* @c: the color with which to draw the triangle
	* @is_filled: whether the triangle should be drawn filled in or wireframe
	*/
	int draw_triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, const RGBA& c, bool is_filled) {
		draw_set_color(c); // Set the desired color

		if (get_options().renderer_type != E_RENDERER::SDL) {
			// Bind the engine vao
			glBindVertexArray(engine->renderer->triangle_vao);

			// Put the list of triangle vertices into the engine vbo
			GLfloat vertices[] = {
				v1.x, v1.y, v1.z,
				v2.x, v2.y, v2.z,
				v3.x, v3.y, v3.z
			};
			glBindBuffer(GL_ARRAY_BUFFER, engine->renderer->triangle_vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

			glUniform1i(engine->renderer->primitive_location, 1); // Enable primitive mode so that the color is correctly applied

			if (!is_filled) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Enable line drawing (i.e. wireframe) mode so that the lines will be drawn correctly
			}

			// Bind the vertices to the vertex array buffer
			glEnableVertexAttribArray(engine->renderer->vertex_location);
			glBindBuffer(GL_ARRAY_BUFFER, engine->renderer->triangle_vbo);
			glVertexAttribPointer(
				engine->renderer->vertex_location,
				3,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
			);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, engine->renderer->triangle_ibo);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0); // Draw the triangle

			// Reset the shader state
			glDisableVertexAttribArray(engine->renderer->vertex_location); // Unbind the vertices
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset the drawing type

			glUniform1i(engine->renderer->primitive_location, 0); // Reset the colorization mode

			glBindVertexArray(0); // Unbind the VAO

			return 0; // Return 0 on success
		} else {
			int r = SDL_RenderDrawLine(engine->renderer->sdl_renderer, v1.x, v1.y, v2.x, v2.y); // Draw the given triangle in the given color
			r |= SDL_RenderDrawLine(engine->renderer->sdl_renderer, v2.x, v2.y, v3.x, v3.y);
			r |= SDL_RenderDrawLine(engine->renderer->sdl_renderer, v1.x, v1.y, v3.x, v3.y);
			return r; // Return the status
		}
	}
	/*
	* draw_line() - Draw a line from (x1, y1) to (x2, y2) in the given color c
	* @v1: the first vertex of the line
	* @v2: the second vertex of the line
	* @c: the color with which to draw the line
	*/
	int draw_line(glm::vec3 v1, glm::vec3 v2, const RGBA& c) {
		draw_set_color(c); // Set the desired color

		if (get_options().renderer_type != E_RENDERER::SDL) {
			return draw_triangle(v1, v2, v2, c, false); // Draw the line as a set of triangles
		} else {
			return SDL_RenderDrawLine(engine->renderer->sdl_renderer, v1.x, v1.y, v2.x, v2.y); // Draw the given line in the given color
		}
	}
	/*
	* draw_line() - Draw a line from (x1, y1) to (x2, y2) in the given color c
	* @x1: the first x-coordinate of the line
	* @y1: the first y-coordinate of the line
	* @x2: the second x-coordinate of the line
	* @y2: the second y-coordinate of the line
	* @c: the color with which to draw the line
	*/
	int draw_line(int x1, int y1, int x2, int y2, const RGBA& c) {
		return draw_line(glm::vec3(x1, y1, 0.0f), glm::vec3(x2, y2, 0.0f), c);
	}
	/*
	* draw_line() - Draw a line from (x1, y1) to (x2, y2) in the current drawing color
	* ! When the function is called without a color, simply call it with the current drawing color
	* @x1: the first x-coordinate of the line
	* @y1: the first y-coordinate of the line
	* @x2: the second x-coordinate of the line
	* @y2: the second y-coordinate of the line
	*/
	int draw_line(int x1, int y1, int x2, int y2) {
		RGBA c = draw_get_color();
		return draw_line(x1, y1, x2, y2, c);
	}
	/*
	* draw_line() - Draw a line from (x1, y1) to (x2, y2) in the given color c
	* ! When the function is called with an E_RGB, simply convert it to an RGBA and call the function again
	* @x1: the first x-coordinate of the line
	* @y1: the first y-coordinate of the line
	* @x2: the second x-coordinate of the line
	* @y2: the second y-coordinate of the line
	* @c: the color with which to draw the line
	*/
	int draw_line(int x1, int y1, int x2, int y2, E_RGB c) {
		return draw_line(x1, y1, x2, y2, get_enum_color(c));
	}
	/*
	* draw_quad() - Draw a quad at the given coordinates
	* @position: the position to draw the quad at
	* @dimensions: the width, height, and depth of the quad
	* @is_filled: whether the quad should be filled or simply an outline
	* @c: the color with which to draw the quad
	*/
	int draw_quad(glm::vec3 position, glm::vec3 dimensions, bool is_filled, const RGBA& c) {
		draw_set_color(c); // Set the desired color

		if (get_options().renderer_type != E_RENDERER::SDL) {
			// Get the width, height, and depth into separate vectors for easy addition
			glm::vec3 w = glm::vec3(dimensions.x, 0.0f, 0.0f);
			glm::vec3 h = glm::vec3(0.0f, dimensions.y, 0.0f);
			glm::vec3 d = glm::vec3(0.0f, 0.0f, -dimensions.z);

			if (is_filled) { // If filling is disabled, only draw a wireframe
				// Draw the front face of the quad
				draw_triangle(position, position+w, position+w+h, c, true);
				draw_triangle(position, position+w+h, position+h, c, true);

				if (dimensions.z != 0) { // Only draw the other faces if the quad has depth
					draw_triangle(position+d, position+w+d, position+w+h+d, c, true);
					draw_triangle(position+d, position+w+h+d, position+h+d, c, true);

					draw_triangle(position, position+d, position+h+d, c, true);
					draw_triangle(position, position+h+d, position+h, c, true);

					draw_triangle(position+w, position+w+d, position+w+h+d, c, true);
					draw_triangle(position+w, position+w+h+d, position+w+h, c, true);

					draw_triangle(position, position+w, position+w+d, c, true);
					draw_triangle(position, position+w+d, position+d, c, true);

					draw_triangle(position+h, position+w+h, position+w+h+d, c, true);
					draw_triangle(position+h, position+w+h+d, position+h+d, c, true);
				}
			} else {
				// Draw the edges of the front face of the quad
				draw_line(position, position+w, c);
				draw_line(position+w, position+w+h, c);
				draw_line(position+w+h, position+h, c);
				draw_line(position+h, position, c);

				if (dimensions.z != 0) { // Only draw the other edges if the quad has depth
					draw_line(position+d, position+w+d, c);
					draw_line(position+w+d, position+w+h+d, c);
					draw_line(position+w+h+d, position+h+d, c);
					draw_line(position+h+d, position+d, c);

					draw_line(position, position+d, c);
					draw_line(position+w, position+w+d, c);
					draw_line(position+w+h, position+w+h+d, c);
					draw_line(position+h, position+h+d, c);
				}
			}

			return 0;
		} else {
			SDL_Rect r = {static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(dimensions.x), static_cast<int>(dimensions.y)};
			if (is_filled) {
				return SDL_RenderFillRect(engine->renderer->sdl_renderer, &r); // Fill the given rectangle with the given color
			} else {
				return SDL_RenderDrawRect(engine->renderer->sdl_renderer, &r); // Draw the given rectangle in the given color
			}
		}
	}
	/*
	* draw_rectangle() - Draw a rectangle at the given coordinates
	* @x: the x-coordinate of the top left of the rectangle
	* @y: the y-coordinate of the top left of the rectangle
	* @w: the width of the rectangle
	* @h: the height of the rectangle
	* @is_filled: whether the rectangle should be filled or simply an outline
	* @c: the color with which to draw the rectangle
	*/
	int draw_rectangle(int x, int y, int w, int h, bool is_filled, const RGBA& c) {
		return draw_quad(glm::vec3(x, y, 0.0f), glm::vec3(w, h, 0.0f), is_filled, c);
	}
	/*
	* draw_rectangle() - Draw a rectangle at the given coordinates
	* ! When the function is called without a color, simply call it with the current drawing color
	* @x: the x-coordinate of the top left of the rectangle
	* @y: the y-coordinate of the top left of the rectangle
	* @w: the width of the rectangle
	* @h: the height of the rectangle
	* @is_filled: whether the rectangle should be filled or simply an outline
	*/
	int draw_rectangle(int x, int y, int w, int h, bool is_filled) {
		RGBA c = draw_get_color();
		return draw_rectangle(x, y, w, h, is_filled, c);
	}
	/*
	* draw_rectangle() - Draw a rectangle at the given coordinates
	* ! When the function is called with an E_RGB, simply convert it to an RGBA and call the function again
	* @x: the x-coordinate of the top left of the rectangle
	* @y: the y-coordinate of the top left of the rectangle
	* @w: the width of the rectangle
	* @h: the height of the rectangle
	* @is_filled: whether the rectangle should be filled or simply an outline
	* @c: the color with which to draw the rectangle
	*/
	int draw_rectangle(int x, int y, int w, int h, bool is_filled, E_RGB c) {
		return draw_rectangle(x, y, w, h, is_filled, get_enum_color(c));
	}
	/*
	* draw_rectangle() - Draw a rectangle at the given coordinates
	* ! When the function is called with an SDL_Rect, simply call it with the SDL_Rect's values
	* @r: the SDL_Rect data of the rectangle
	* @is_filled: whether the rectangle should be filled or simply an outline
	* @c: the color with which to draw the rectangle
	*/
	int draw_rectangle(const SDL_Rect& r, bool is_filled, const RGBA& c) {
		return draw_rectangle(r.x, r.y, r.w, r.h, is_filled, c);
	}

	/*
	* draw_set_color() - Set the current drawing color to the given value
	* @new_color: the new RGBA with which to draw and clear the screen
	*/
	int draw_set_color(const RGBA& new_color) {
		// Set color to the new color
		engine->color->r = new_color.r;
		engine->color->g = new_color.g;
		engine->color->b = new_color.b;
		engine->color->a = new_color.a;

		if (get_options().renderer_type != E_RENDERER::SDL) {
			glm::vec4 uc = glm::vec4(engine->color->r, engine->color->g, engine->color->b, engine->color->a);
			uc /= 255.0f;
			glClearColor(uc.r, uc.g, uc.b, uc.a); // Set the OpenGL clear and draw colors as floats from [0.0, 1.0]
			glUniform4fv(engine->renderer->colorize_location, 1, glm::value_ptr(uc)); // Change the fragment to the given color
		} else {
			return SDL_SetRenderDrawColor(engine->renderer->sdl_renderer, new_color.r, new_color.g, new_color.b, new_color.a); // Set the SDL draw color as Uint8's from [0, 255]
		}

		return 0;
	}
	/*
	* draw_set_color() - Set the current drawing color to the given value
	* ! When the function is called with an E_RGB, simply convert it and call the function again
	* @new_color: the new color with which to draw and clear the screen
	*/
	int draw_set_color(E_RGB new_color) {
		return draw_set_color(get_enum_color(new_color));
	}
	/*
	* draw_get_color() - Return the current color and set the drawing color to ensure consistency
	*/
	RGBA draw_get_color() {
		RGBA c = {0, 0, 0, 0};

		if (get_options().renderer_type != E_RENDERER::SDL) {
			glm::vec4 uc = glm::vec4(engine->color->r, engine->color->g, engine->color->b, engine->color->a);
			uc /= 255.0f;
			glClearColor(uc.r, uc.g, uc.b, uc.a); // Set the OpenGL clear and draw colors as floats from [0.0, 1.0]
			glUniform4fv(engine->renderer->colorize_location, 1, glm::value_ptr(uc)); // Change the fragment to the given color
		} else {
			SDL_GetRenderDrawColor(engine->renderer->sdl_renderer, &c.r, &c.g, &c.b, &c.a); // Get the current SDL renderer color

			if ((engine->color->r != c.r)||(engine->color->g != c.g)||(engine->color->b != c.b)||(engine->color->a != c.a)) { // Only set the color if it needs to be changed
				SDL_SetRenderDrawColor(engine->renderer->sdl_renderer, engine->color->r, engine->color->g, engine->color->b, engine->color->a); // Set the SDL draw color as Uint8's from [0, 255]
			}
		}

		return *(engine->color); // Return the current draw color
	}
	/*
	* draw_set_blend() - Set the current drawing blend mode to the given type
	* ! After calling this function it is the user's job to reset the blend mode to the previous state
	* @blend: the new blend mode to use
	*/
	int draw_set_blend(SDL_BlendMode blend) {
		if (get_options().renderer_type == E_RENDERER::SDL) {
			SDL_SetRenderDrawBlendMode(engine->renderer->sdl_renderer, blend);
		}
		return 0;
	}
	/*
	* draw_get_blend() - Return the current drawing blend mode
	*/
	SDL_BlendMode draw_get_blend() {
		SDL_BlendMode blend = SDL_BLENDMODE_BLEND;
		if (get_options().renderer_type == E_RENDERER::SDL) {
			SDL_GetRenderDrawBlendMode(engine->renderer->sdl_renderer, &blend);
		}
		return blend;
	}

	/*
	* get_pixel_color() - Return the color of the given pixel in the window
	* ! This function is somewhat slow and should be used sparingly
	* @x: the x-coordinate of the pixel
	* @y: the y-coordinate of the pixel
	*/
	RGBA get_pixel_color(int x, int y) {
		if (get_options().renderer_type != E_RENDERER::SDL) {
			unsigned char* pixel = new unsigned char[4]; // Allocate 4 bytes per pixel for RGBA
			glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel); // Read the screen pixel into the array

			RGBA c = {pixel[0], pixel[1], pixel[2], pixel[3]}; // Convert the pixel data into an RGBA

			delete[] pixel; // Free the pixel data

			return c; // Return the pixel color
		} else {
			SDL_Surface* screenshot = SDL_CreateRGBSurface(0, engine->width, engine->height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000); // Create a surface from the screen pixels
			SDL_RenderReadPixels(engine->renderer->sdl_renderer, nullptr, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

			RGBA c;
			SDL_GetRGBA(static_cast<Uint32*>(screenshot->pixels)[x+y*engine->height], screenshot->format, &c.r, &c.g, &c.b, &c.a); // Fetch the pixel data from the surface into an RGBA

			SDL_FreeSurface(screenshot); // Free the surface

			return c; // Return the pixel color
		}
	}
	/*
	* save_screenshot() - Save a bitmap (.bmp) of the window to the given filename
	* ! This function is somewhat slow and should be used sparingly
	* @filename: the location at which to save the bitmap
	*/
	int save_screenshot(const std::string& filename) {
		if (get_options().is_headless) {
			return -1; // Return -1 when in headless mode
		}

		std::string fn (filename);
		if (file_exists(fn)) { // If the file already exists, append a timestamp
			fn = file_plainname(fn) + "-" + bee_itos(time(nullptr)) + file_extname(fn);
			if (file_exists(fn)) { // If the appended file already exists, abort
				messenger::send({"engine"}, E_MESSAGE::WARNING, "Failed to save screenshot: files already exist: \"" + filename + "\" and \"" + fn + "\"");
				return -2; // Return -2 on filename error
			}
		}

		if (get_options().renderer_type != E_RENDERER::SDL) {
			unsigned char* upsidedown_pixels = new unsigned char[engine->width*engine->height*4]; // Allocate 4 bytes per pixel for RGBA
			glReadPixels(0, 0, engine->width, engine->height, GL_RGBA, GL_UNSIGNED_BYTE, upsidedown_pixels); // Read the screen pixels into the array

			unsigned char* pixels = new unsigned char[engine->width*engine->height*4];
			for (size_t i=0; i<engine->height; i++) { // Reverse the order of the rows from glReadPixels() because the OpenGL origin is bottom-left and the SDL origin is top-left
				for (size_t e=0; e<engine->width; e++) {
					for (size_t o=0; o<4; o++) {
						pixels[i*engine->width*4 + e*4 + o] = upsidedown_pixels[(engine->height-i)*engine->width*4 + e*4 + o];
					}
				}
			}

			SDL_Surface* screenshot  = SDL_CreateRGBSurfaceFrom(pixels, engine->width, engine->height, 8*4, engine->width*4, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000); // Create a surface from the screen pixels
			SDL_SaveBMP(screenshot, fn.c_str()); // Save the surface to the given filename as a bitmap

			SDL_FreeSurface(screenshot); // Free the surface and pixel data
			delete[] pixels;
			delete[] upsidedown_pixels;
		} else {
			SDL_Surface* screenshot = SDL_CreateRGBSurface(0, engine->width, engine->height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000); // Create a surface from the screen pixels
			SDL_RenderReadPixels(engine->renderer->sdl_renderer, nullptr, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

			SDL_SaveBMP(screenshot, fn.c_str()); // Save the surface to the given filename as a bitmap

			SDL_FreeSurface(screenshot); // Free the surface
		}

		if (filename != fn) {
			return 1; // Return 1 on successful save
		}

		return 0; // Return 0 on successfull save and filename
	}
}

#endif // BEE_RENDER_DRAWING
