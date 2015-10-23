/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_GAME_DRAW
#define _BEE_GAME_DRAW 1

#include "../game.hpp"

BEE::RGBA BEE::get_enum_color(rgba_t c, Uint8 a) {
	switch (c) {
		case c_aqua: return {0, 255, 255, a};
		case c_black: return {0, 0, 0, a};
		case c_blue: return {0, 0, 255, a};
		case c_dkgray: return {64, 64, 64, a};
		case c_fuchsia: return {255, 0, 255, a};
		case c_gray: return {128, 128, 128, a};
		case c_green: return {0, 255, 0, a};
		//case c_lime: return {255, 255, 255, a};
		case c_ltgray: return {192, 192, 192, a};
		//case c_maroon: return {255, 255, 255, a};
		//case c_navy: return {255, 255, 255, a};
		//case c_olive: return {255, 255, 255, a};
		case c_orange: return {255, 128, 0, a};
		case c_purple: return {128, 0, 255, a};
		case c_red: return {255, 0, 0, a};
		//case c_silver: return {255, 255, 255, a};
		//case c_teal: return {255, 255, 255, a};
		case c_white: return {255, 255, 255, a};
		case c_yellow: return {255, 255, 0, a};
		default: return {0, 0, 0, a};
	}
}
BEE::RGBA BEE::get_enum_color(rgba_t c) {
	return get_enum_color(c, 255);
}

int BEE::draw_point(int x, int y) {
	return SDL_RenderDrawPoint(renderer, x, y);
}
int BEE::draw_line(int x1, int y1, int x2, int y2) {
	return SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}
int BEE::draw_line(int x1, int y1, int x2, int y2, RGBA color) {
	RGBA c = draw_get_color();
	draw_set_color(color);
	int r = draw_line(x1, y1, x2, y2);
	draw_set_color(c);
	return r;
}
int BEE::draw_line(int x1, int y1, int x2, int y2, rgba_t color) {
	return draw_line(x1, y1, x2, y2, get_enum_color(color));
}
int BEE::draw_rectangle(int x, int y, int w, int h, bool is_filled) {
	SDL_Rect rect = {x, y, w, h};
	if (is_filled) {
		return SDL_RenderFillRect(renderer, &rect);
	} else {
		return SDL_RenderDrawRect(renderer, &rect);
	}
}
int BEE::draw_rectangle(int x, int y, int w, int h, bool is_filled, RGBA color) {
	RGBA c = draw_get_color();
	draw_set_color(color);
	int r = draw_rectangle(x, y, w, h, is_filled);
	draw_set_color(c);
	return r;
}
int BEE::draw_rectangle(int x, int y, int w, int h, bool is_filled, rgba_t color) {
	return draw_rectangle(x, y, w, h, is_filled, get_enum_color(color));
}
int BEE::draw_set_color(RGBA new_color) {
	return SDL_SetRenderDrawColor(renderer, new_color.r, new_color.g, new_color.b, new_color.a);
}
int BEE::draw_set_color(rgba_t new_color) {
	return draw_set_color(get_enum_color(new_color));
}
BEE::RGBA BEE::draw_get_color() {
	Uint8 r, g, b, a;
	SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a);
	RGBA c = {r, g, b, a};
	return c;
}
BEE::RGBA BEE::get_pixel_color(int x, int y) {
	SDL_Surface *screenshot = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

	RGBA color;
	SDL_GetRGBA(((Uint32*)screenshot->pixels)[x+y*height], screenshot->format, &color.r, &color.g, &color.b, &color.a);

	SDL_FreeSurface(screenshot);

	return color;
}
int BEE::save_screenshot(std::string filename) { // Slow, use sparingly
	if (options->is_opengl) {
		/*unsigned char* pixels = new unsigned char[width*height*4]; // 4 bytes for RGBA
		glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		SDL_Surface* screenshot  = SDL_CreateRGBSurfaceFrom(pixels, width, height, 8*4, width*4, 0,0,0,0);
		SDL_SaveBMP(screenshot), filename.c_str());

		SDL_FreeSurface(screenshot);
		delete [] pixels;*/
	} else {
		SDL_Surface *screenshot = SDL_CreateRGBSurface(0, width, height, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
		SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, screenshot->pixels, screenshot->pitch);

		SDL_SaveBMP(screenshot, filename.c_str());

		SDL_FreeSurface(screenshot);
	}

	return 0;
}

#endif // _BEE_GAME_DRAW
