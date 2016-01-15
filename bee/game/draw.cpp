/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_DRAW
#define _BEE_GAME_DRAW 1

#include "../game.hpp" // Include the engine headers

/*
* BEE::get_enum_color() - Return the BEE::RGBA value of the given rgba_t enumeration
* @c: the rgba_t enumeration to convert
* @a: the alpha value to use
*/
BEE::RGBA BEE::get_enum_color(rgba_t c, Uint8 a) const {
	// Return a BEE::RGBA value with the given alpha value
	switch (c) {
		case c_cyan: case c_aqua: return {0, 255, 255, a};
		case c_blue: return {0, 0, 255, a};
		case c_dkgray: return {64, 64, 64, a};
		case c_magenta: case c_fuchsia: return {255, 0, 255, a};
		case c_gray: return {128, 128, 128, a};
		case c_green: return {0, 128, 0, a}; // Even though green is technically g=255, that color is called lime because it is quite bright
		case c_lime: return {0, 255, 0, a};
		case c_silver: case c_ltgray: return {192, 192, 192, a};
		case c_maroon: return {128, 0, 0, a};
		case c_navy: return {0, 0, 128, a};
		case c_olive: return {128, 128, 0, a};
		case c_orange: return {255, 128, 0, a};
		case c_purple: return {128, 0, 255, a};
		case c_red: return {255, 0, 0, a};
		case c_teal: return {0, 128, 128, a};
		case c_white: return {255, 255, 255, a};
		case c_yellow: return {255, 255, 0, a};
		case c_black: default: return {0, 0, 0, a}; // Return black if the enumeration is unknown
	}
}
/*
* BEE::get_enum_color() - Return the BEE::RGBA value of the given rgba_t enumeration
* ! When the function is called without an alpha value, simply call it with full opacity
* @c: the rgba_t enumeration to convert
*/
BEE::RGBA BEE::get_enum_color(rgba_t c) const {
	return get_enum_color(c, 255);
}

/*
* BEE::convert_view_coords() - Convert the given coordinates into the viewport coordinates so that drawing can be done relative to the current view
* @x: the x-coordinate to convert
* @y: the y-coordinate to convert
*/
int BEE::convert_view_coords(int& x, int& y) const {
	if (get_current_room()->get_is_views_enabled()) { // Only actually convert the coordinates if views are currently enabled
		// Add the current viewport's offset coordinates to the given coordinates
		x += get_current_room()->get_current_view()->view_x;
		y += get_current_room()->get_current_view()->view_y;
	}
	return 0;
}
/*
* BEE::convert_window_coords() - Convert the given coordinates into the window coordinates so that drawing can be done relative to the window
* @x: the x-coordinate to convert
* @y: the y-coordinate to convert
*/
int BEE::convert_window_coords(int& x, int& y) const {
	if (get_current_room()->get_is_views_enabled()) { // Only actually convert the coordinates if views are currently enabled
		// Subtract the current viewport's offset coordinates from the given coordinates
		x -= get_current_room()->get_current_view()->view_x;
		y -= get_current_room()->get_current_view()->view_y;
	}
	return 0;
}

/*
* BEE::draw_point() - Draw a single pixel at the given coordinates
* @x: the x-coordinate at which to draw the pixel
* @y: the y-coordinate at which to draw the pixel
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_point(int x, int y, bool is_hud) {
	if (!is_hud) { // Only convert the coordinates if they should not be drawn relative to the window
		convert_view_coords(x, y);
	}

	if (options->is_opengl) {
		// Right now all primitives are drawn using the fixed function OpenGL pipeline
		glBegin(GL_POINTS); // Add point vertex
			glVertex2f(x, y);
		glEnd();
		return 1;
	} else {
		return SDL_RenderDrawPoint(renderer, x, y); // Draw the given point
	}
}
/*
* BEE::draw_line() - Draw a line from (x1, y1) to (x2, y2) in the given color c
* @x1: the first x-coordinate of the line
* @y1: the first y-coordinate of the line
* @x2: the second x-coordinate of the line
* @y2: the second y-coordinate of the line
* @c: the color with which to draw the line
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_line(int x1, int y1, int x2, int y2, const RGBA& c, bool is_hud) {
	if (!is_hud) { // Only convert the coordinates if they should not be drawn relative to the window
		convert_view_coords(x1, y1);
		convert_view_coords(x2, y2);
	}

	if (options->is_opengl) {
		// Right now all primitives are drawn using the fixed function OpenGL pipeline
		glm::vec4 uc = glm::vec4(c.r/255.0, c.g/255.0, c.b/255.0, c.a/255.0); // Change the fragment to the given color
		glUniform4fv(colorize_location, 1, glm::value_ptr(uc));

		glBegin(GL_LINES); // Add line vertices
			glVertex2f(x1, y1);
			glVertex2f(x2, y2);
		glEnd();

		return 1;
	} else {
		draw_set_color(c);
		return SDL_RenderDrawLine(renderer, x1, y1, x2, y2); // Draw the given point in the given color
	}
}
/*
* BEE::draw_line() - Draw a line from (x1, y1) to (x2, y2) in the current drawing color
* ! When the function is called without a color, simply call it with the current drawing color
* @x1: the first x-coordinate of the line
* @y1: the first y-coordinate of the line
* @x2: the second x-coordinate of the line
* @y2: the second y-coordinate of the line
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_line(int x1, int y1, int x2, int y2, bool is_hud) {
	RGBA c = draw_get_color();
	return draw_line(x1, y1, x2, y2, c, is_hud);
}
/*
* BEE::draw_line() - Draw a line from (x1, y1) to (x2, y2) in the given color c
* ! When the function is called with an rgba_t, simply convert it to an RGBA and call the function again
* @x1: the first x-coordinate of the line
* @y1: the first y-coordinate of the line
* @x2: the second x-coordinate of the line
* @y2: the second y-coordinate of the line
* @c: the color with which to draw the line
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_line(int x1, int y1, int x2, int y2, rgba_t c, bool is_hud) {
	return draw_line(x1, y1, x2, y2, get_enum_color(c), is_hud);
}
/*
* BEE::draw_line() - Draw a line along the Line l in the given color c
* ! When the function is called with a Line structure, simply call it with the Line's coordinates
* @l: the Line data of the line
* @c: the color with which to draw the line
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_line(const Line& l, const RGBA& c, bool is_hud) {
	return draw_line(l.x1, l.y1, l.x2, l.y2, c, is_hud);
}
/*
* BEE::draw_rectangle() - Draw a rectangle at the given coordinates
* @x: the x-coordinate of the top left of the rectangle
* @y: the y-coordinate of the top left of the rectangle
* @w: the width of the rectangle
* @h: the height of the rectangle
* @is_filled: whether the rectangle should be filled or simply an outline
* @c: the color with which to draw the rectangle
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_rectangle(int x, int y, int w, int h, bool is_filled, const RGBA& c, bool is_hud) {
	if (!is_hud) { // Only convert the coordinates if they should not be drawn relative to the window
		convert_view_coords(x, y);
	}

	if (options->is_opengl) {
		// Right now all primitives are drawn using the fixed function OpenGL pipeline
		glm::vec4 uc = glm::vec4(c.r/255.0, c.g/255.0, c.b/255.0, c.a/255.0); // Change the fragment to the given color
		glUniform4fv(colorize_location, 1, glm::value_ptr(uc));

		// Set polygon fill mode
		if (is_filled) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		} else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}

		glBegin(GL_POLYGON); // Add rectangle vertices
			glVertex2f(x, y);
			glVertex2f(x+w, y);
			glVertex2f(x+w, y+h);
			glVertex2f(x, y+h);
		glEnd();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Reset polygon to fill so that textures are drawn correctly

		return 1;
	} else {
		draw_set_color(c);
		SDL_Rect r = {x, y, w, h};
		if (is_filled) {
			return SDL_RenderFillRect(renderer, &r); // Fill the given rectangle with the given color
		} else {
			return SDL_RenderDrawRect(renderer, &r); // Draw the given rectangle in the given color
		}
	}
}
/*
* BEE::draw_rectangle() - Draw a rectangle at the given coordinates
* ! When the function is called without a color, simply call it with the current drawing color
* @x: the x-coordinate of the top left of the rectangle
* @y: the y-coordinate of the top left of the rectangle
* @w: the width of the rectangle
* @h: the height of the rectangle
* @is_filled: whether the rectangle should be filled or simply an outline
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_rectangle(int x, int y, int w, int h, bool is_filled, bool is_hud) {
	RGBA c = draw_get_color();
	return draw_rectangle(x, y, w, h, is_filled, c, is_hud);
}
/*
* BEE::draw_rectangle() - Draw a rectangle at the given coordinates
* ! When the function is called with an rgba_t, simply convert it to an RGBA and call the function again
* @x: the x-coordinate of the top left of the rectangle
* @y: the y-coordinate of the top left of the rectangle
* @w: the width of the rectangle
* @h: the height of the rectangle
* @is_filled: whether the rectangle should be filled or simply an outline
* @c: the color with which to draw the rectangle
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_rectangle(int x, int y, int w, int h, bool is_filled, rgba_t c, bool is_hud) {
	return draw_rectangle(x, y, w, h, is_filled, get_enum_color(c), is_hud);
}
/*
* BEE::draw_rectangle() - Draw a rectangle at the given coordinates
* ! When the function is called with an SDL_Rect, simply call it with the SDL_Rect's values
* @r: the SDL_Rect data of the rectangle
* @is_filled: whether the rectangle should be filled or simply an outline
* @c: the color with which to draw the rectangle
* @is_hud: whether the coordinates should be left unconverted or not
*/
int BEE::draw_rectangle(const SDL_Rect& r, bool is_filled, const RGBA& c, bool is_hud) {
	return draw_rectangle(r.x, r.y, r.w, r.h, is_filled, c, is_hud);
}

/*
* BEE::draw_set_color() - Set the current drawing color to the given value
* @new_color: the new RGBA with which to draw and clear the screen
*/
int BEE::draw_set_color(const RGBA& new_color) {
	// Set BEE::color to the new color
	color->r = new_color.r;
	color->g = new_color.g;
	color->b = new_color.b;
	color->a = new_color.a;

	if (options->is_opengl) {
		glClearColor(new_color.r/255.0f, new_color.g/255.0f, new_color.b/255.0f, new_color.a/255.0f); // Set the OpenGL clear and draw colors as floats from [0.0, 1.0]
		glColor4f(new_color.r/255.0f, new_color.g/255.0f, new_color.b/255.0f, new_color.a/255.0f);
		return 0;
	} else {
		return SDL_SetRenderDrawColor(renderer, new_color.r, new_color.g, new_color.b, new_color.a); // Set the SDL draw color as Uint8's from [0, 255]
	}
}
/*
* BEE::draw_set_color() - Set the current drawing color to the given value
* ! When the function is called with an rgba_t, simply convert it and call the function again
* @new_color: the new color with which to draw and clear the screen
*/
int BEE::draw_set_color(rgba_t new_color) {
	return draw_set_color(get_enum_color(new_color));
}
/*
* BEE::draw_get_color() - Return the current BEE::color and set the drawing color to ensure consistency
*/
BEE::RGBA BEE::draw_get_color() const {
	RGBA c = {0, 0, 0, 0};

	if (options->is_opengl) {
		glClearColor(color->r/255.0f, color->g/255.0f, color->b/255.0f, color->a/255.0f); // Set the OpenGL clear and draw colors as floats from [0.0, 1.0]
		glColor4f(color->r/255.0f, color->g/255.0f, color->b/255.0f, color->a/255.0f);
	} else {
		SDL_GetRenderDrawColor(renderer, &c.r, &c.g, &c.b, &c.a); // Get the current SDL renderer color

		if ((color->r != c.r)||(color->g != c.g)||(color->b != c.b)||(color->a != c.a)) { // Only set the color if it needs to be changed
			SDL_SetRenderDrawColor(renderer, color->r, color->g, color->b, color->a); // Set the SDL draw color as Uint8's from [0, 255]
		}
	}

	return *color; // Return the current draw color
}
/*
* BEE::get_pixel_color() - Return the color of the given pixel in the window
* ! This function is somewhat slow and should be used sparingly
* @x: the x-coordinate of the pixel
* @y: the y-coordinate of the pixel
*/
BEE::RGBA BEE::get_pixel_color(int x, int y) const {
	if (options->is_opengl) {
		unsigned char* pixel = new unsigned char[4]; // Allocate 4 bytes per pixel for RGBA
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel); // Read the screen pixel into the array

		RGBA c = {pixel[0], pixel[1], pixel[2], pixel[3]}; // Convert the pixel data into an RGBA

		delete[] pixel; // Free the pixel data

		return c; // Return the pixel color
	} else {
		SDL_Surface *screenshot = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000); // Create a surface from the screen pixels
		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

		RGBA c;
		SDL_GetRGBA(((Uint32*)screenshot->pixels)[x+y*height], screenshot->format, &c.r, &c.g, &c.b, &c.a); // Fetch the pixel data from the surface into an RGBA

		SDL_FreeSurface(screenshot); // Free the surface

		return c; // Return the pixel color
	}
}
/*
* BEE::save_screenshot() - Save a bitmap (.bmp) of the window to the given filename
* ! This function is somewhat slow and should be used sparingly
* @filename: the location at which to save the bitmap
*/
int BEE::save_screenshot(const std::string& filename) const {
	if (options->is_opengl) {
		unsigned char* pixels = new unsigned char[width*height*4]; // Allocate 4 bytes per pixel for RGBA
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels); // Read the screen pixels into the array

		SDL_Surface* screenshot  = SDL_CreateRGBSurfaceFrom(pixels, width, height, 8*4, width*4, 0,0,0,0); // Create a surface from the screen pixels
		SDL_SaveBMP(screenshot, filename.c_str()); // Save the surface to the given filename as a bitmap

		SDL_FreeSurface(screenshot); // Free the surface and pixel data
		delete[] pixels;
	} else {
		SDL_Surface *screenshot = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000); // Create a surface from the screen pixels
		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

		SDL_SaveBMP(screenshot, filename.c_str()); // Save the surface to the given filename as a bitmap

		SDL_FreeSurface(screenshot); // Free the surface
	}

	return 0; // Return 0 on success
}

#endif // _BEE_GAME_DRAW
