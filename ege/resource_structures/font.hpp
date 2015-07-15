/*
* Copyright (c) 2015 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of EGE.
* EGE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _EGE_FONT_H
#define _EGE_FONT_H 1

#include <iostream>

class Font: public Resource {
		// Add new variables to the print() debugging method
		int id;
		std::string name;
		std::string font_path;
		int font_size;

		TTF_Font* font;
		bool is_loaded;
		SDL_Texture* texture;
		int width, height;
	public:
		Font();
		Font(std::string, std::string, int);
		~Font();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		int get_font_size();

		int set_name(std::string);
		int set_path(std::string);
		int set_font_size(int);

		int load();
		int free();
		int draw(int, int, std::string, SDL_Color);
		int draw(int, int, std::string);
		int reset_texture();

		int get_string_width(std::string, int);
		int get_string_width(std::string);
};
Font::Font () {
	id = -1;
	reset();
}
Font::Font (std::string new_name, std::string path, int new_font_size) {
	id = -1;
	is_loaded = false;
	reset();

	add_to_resources("resources/fonts/"+path);
	if (id < 0) {
		std::cerr << "Failed to add font resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
	set_font_size(new_font_size);
}
Font::~Font() {
	resource_list.fonts.remove_resource(id);
}
int Font::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == font_path) {
			return 1;
		}
		resource_list.fonts.remove_resource(id);
		id = -1;
	} else {
		for (auto i : resource_list.fonts.resources) {
			if ((i.second != NULL)&&(i.second->get_path() == path)) {
				list_id = i.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = resource_list.fonts.add_resource(this);
	}
	resource_list.fonts.set_resource(id, this);

	return 0;
}
int Font::reset() {
	if (is_loaded) {
		free();
	}

	name = "";
	font_path = "";
	font_size = 16;

	font = NULL;
	is_loaded = false;
	texture = NULL;
	width = 0;
	height = 0;

	return 0;
}
int Font::print() {
	std::cout <<
	"Font { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	font_path	" << font_path <<
	"\n	font_size	" << font_size <<
	"\n	font		" << font <<
	"\n	texture		" << texture <<
	"\n}\n";

	return 0;
}
int Font::get_id() {
	return id;
}
std::string Font::get_name() {
	return name;
}
std::string Font::get_path() {
	return font_path;
}
int Font::get_font_size() {
	return font_size;
}
int Font::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int Font::set_path(std::string path) {
	add_to_resources("resources/fonts/"+path);
	font_path = "resources/fonts/"+path;
	return 0;
}
int Font::set_font_size(int new_font_size) {
	font_size = new_font_size;
	return 0;
}
int Font::load() {
	if (!is_loaded) {
		font = TTF_OpenFont(font_path.c_str(), font_size);
		if (font == NULL) {
			std::cerr << "Failed to load font " << font_path << ": " << TTF_GetError() << "\n";
			return 1;
		}

		is_loaded = true;
	}

	return 0;
}
int Font::free() {
	if (is_loaded) {
		reset_texture();

		TTF_CloseFont(font);
		font = NULL;

		is_loaded = false;
	}

	return 0;
}
int Font::draw(int x, int y, std::string text, SDL_Color color) {
	if (is_loaded) {
		if (text.size() > 0) {
			SDL_Surface* tmp_surface;
			tmp_surface = TTF_RenderText_Solid(font, text.c_str(), color);
			if (tmp_surface == NULL) {
				std::cerr << "Failed to draw with font " << name << ": " << TTF_GetError() << "\n";
				return 1;
			}

			texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
			if (texture == NULL) {
				std::cerr << "Failed to create texture from surface: " << SDL_GetError() << "\n";
				return 1;
			}

			SDL_FreeSurface(tmp_surface);
			SDL_QueryTexture(texture, NULL, NULL, &width, &height);

			SDL_Rect rect;
			rect.x = x;
			rect.y = y;
			rect.w = width;
			rect.h = height;
			SDL_RenderCopy(game->renderer, texture, NULL, &rect);

			reset_texture();
		}

		return 0;
	}

	std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
	return 1;
}
int Font::draw(int x, int y, std::string text) {
	SDL_Color color = {0, 0, 0, 1};
	return draw(x, y, text, color);
}
int Font::reset_texture() {
	SDL_DestroyTexture(texture);
	texture = NULL;
	width = 0;
	height = 0;

	return 0;
}

int Font::get_string_width(std::string text, int size) {
	if (is_loaded) {
		int w = 0;
		if (text.size() > 0) {
			SDL_Surface* tmp_surface;
			tmp_surface = TTF_RenderText_Solid(font, text.c_str(), {0, 0, 0, 1});
			if (tmp_surface == NULL) {
				std::cerr << "Failed to draw with font " << name << ": " << TTF_GetError() << "\n";
				return 1;
			}
			w = tmp_surface->w;
			SDL_FreeSurface(tmp_surface);
		}

		return w;
	}

	std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
	return -1;
}
int Font::get_string_width(std::string text) {
	return get_string_width(text, font_size);
}

#endif // _EGE_FONT_H
