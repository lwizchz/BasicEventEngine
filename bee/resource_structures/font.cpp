/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_FONT
#define _BEE_FONT 1

#include "font.hpp"

TextData::TextData(BEE::Sprite* new_sprite, std::string new_text) {
	sprite.insert(std::make_pair(0, new_sprite));
	text = new_text;
}
TextData::~TextData() {
	for (auto& s : sprite) {
		s.second->free();
	}
	sprite.clear();
	text = "";
}

BEE::Font::Font () {
	if (BEE::resource_list->fonts.game != NULL) {
		game = BEE::resource_list->fonts.game;
	}
	reset();
}
BEE::Font::Font (std::string new_name, std::string path, int new_font_size, bool new_is_sprite) {
	reset();

	add_to_resources("resources/fonts/"+path);
	if (id < 0) {
		std::cerr << "Failed to add font resource: " << path << "\n";
		throw(-1);
	}

	set_name(new_name);
	set_path(path);
	set_font_size(new_font_size);
	is_sprite = new_is_sprite;
}
BEE::Font::~Font() {
	free();
	BEE::resource_list->fonts.remove_resource(id);
}
int BEE::Font::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == font_path) {
			return 1;
		}
		BEE::resource_list->fonts.remove_resource(id);
		id = -1;
	} else {
		for (auto i : BEE::resource_list->fonts.resources) {
			if ((i.second != NULL)&&(i.second->get_path() == path)) {
				list_id = i.first;
				break;
			}
		}
	}

	if (list_id >= 0) {
		id = list_id;
	} else {
		id = BEE::resource_list->fonts.add_resource(this);
	}
	BEE::resource_list->fonts.set_resource(id, this);

	if (BEE::resource_list->fonts.game != NULL) {
		game = BEE::resource_list->fonts.game;
	}

	return 0;
}
int BEE::Font::reset() {
	free();

	name = "";
	font_path = "";
	font_size = 16;
	style = TTF_STYLE_NORMAL;
	lineskip = 0;

	font = NULL;
	is_loaded = false;
	has_draw_failed = false;

	sprite_font = NULL;
	is_sprite = false;

	return 0;
}
int BEE::Font::print() {
	std::cout <<
	"Font { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	font_path	" << font_path <<
	"\n	font_size	" << font_size <<
	"\n	style		" << style <<
	"\n	lineskip	" << lineskip <<
	"\n	font		" << font <<
	"\n}\n";

	return 0;
}
int BEE::Font::get_id() {
	return id;
}
std::string BEE::Font::get_name() {
	return name;
}
std::string BEE::Font::get_path() {
	return font_path;
}
int BEE::Font::get_font_size() {
	return font_size;
}
int BEE::Font::get_style() {
	return style;
}
int BEE::Font::get_lineskip() {
	return lineskip;
}
int BEE::Font::get_lineskip_default() {
	if (is_loaded) {
		return TTF_FontLineSkip(font);
	}
	std::cerr << "Failed to get default lineskip, font not loaded: " << name << "\n";
	return -1;
}
std::string BEE::Font::get_fontname() {
	std::string fontname = "";
	char* familyname = TTF_FontFaceFamilyName(font);
	char* stylename = TTF_FontFaceStyleName(font);
	fontname += familyname;
	fontname += " ";
	fontname += stylename;
	return fontname;
}

int BEE::Font::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Font::set_path(std::string path) {
	add_to_resources("resources/fonts/"+path);
	font_path = "resources/fonts/"+path;
	return 0;
}
int BEE::Font::set_font_size(int new_font_size) {
	font_size = new_font_size;
	return 0;
}
int BEE::Font::set_style(int new_style) {
	if (is_loaded)	{
		style = new_style; // bitmask of TTF_STYLE_BOLD, ITALIC, UNDERLINE, STRIKETHROUGH, and NORMAL
		TTF_SetFontStyle(font, style);
		return 0;
	}

	std::cerr << "Failed to set font style, font not loaded: " << name << "\n";
	return 1;
}
int BEE::Font::set_lineskip(int new_lineskip) {
	lineskip = new_lineskip;
	return 0;
}

int BEE::Font::load() {
	if (!is_loaded) {
		if (is_sprite) {
			sprite_font = new Sprite("spr_"+get_name(), font_path);
			if (!sprite_font->load()) {
				std::cerr << "Failed to load font " << font_path << ": " << SDL_GetError() << "\n";
				has_draw_failed = true;
			}

			is_loaded = true;
			has_draw_failed = false;
		} else {
			if (game->options->is_opengl) {
				std::cerr << "Please note that TTF fast font rendering is currently broken in OpenGL\n";
			}

			font = TTF_OpenFont(font_path.c_str(), font_size);
			if (font == NULL) {
				std::cerr << "Failed to load font " << font_path << ": " << TTF_GetError() << "\n";
				return 1;
			}

			is_loaded = true;
			has_draw_failed = false;
		}
	}

	return 0;
}
int BEE::Font::free() {
	if (is_loaded) {
		if (is_sprite) {
			delete sprite_font;
			sprite_font = NULL;
		} else {
			TTF_CloseFont(font);
			font = NULL;
		}

		is_loaded = false;
	}

	return 0;
}

TextData* BEE::Font::draw_internal(int x, int y, std::string text, RGBA color) {
	if (is_loaded) {
		if (text.size() > 0) {
			text = string_replace(text, "\t", "    ");

			SDL_Surface* tmp_surface;
			tmp_surface = TTF_RenderUTF8_Blended(font, text.c_str(), {color.r, color.g, color.b, color.a}); // Slow but pretty
			if (tmp_surface == NULL) {
				std::cerr << "Failed to draw with font " << name << ": " << TTF_GetError() << "\n";
				return NULL;
			}

			Sprite* tmp_sprite = new Sprite();

			tmp_sprite->load_from_surface(tmp_surface);
			tmp_sprite->draw(x, y, 0, false);

			SDL_FreeSurface(tmp_surface);

			TextData* textdata = new TextData(tmp_sprite, text);

			return textdata;
		}

		return NULL;
	}

	if (!has_draw_failed) {
		std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
		has_draw_failed = true;
	}
	return NULL;
}
TextData* BEE::Font::draw(int x, int y, std::string text, RGBA color) {
	if (is_loaded) {
		TextData *r = NULL, *textdata = NULL;
		std::map<int,std::string> lines = handle_newlines(text);

		if (lineskip == 0) {
			lineskip = TTF_FontLineSkip(font);
		}

		for (auto& l : lines) {
			r = draw_internal(x, y+lineskip*l.first, l.second, color);
			if (r != NULL) {
				if (textdata == NULL) {
					textdata = r;
				} else {
					textdata->sprite.insert(std::make_pair(textdata->sprite.size(), r->sprite[0]));
					delete r;
				}
			}
		}
		return textdata;
	}

	if (!has_draw_failed) {
		std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
		has_draw_failed = true;
	}
	return NULL;
}
TextData* BEE::Font::draw(int x, int y, std::string text) {
	return draw(x, y, text, {0, 0, 0, 255});
}
TextData* BEE::Font::draw(TextData* textdata, int x, int y, std::string text, RGBA color) {
	if (is_loaded) {
		if ((textdata != NULL)&&(textdata->text == text)) {
			std::map<int,std::string> lines = handle_newlines(text);
			for (auto& l : lines) {
				textdata->sprite[l.first]->draw(x, y, 0, false);
			}
			return textdata;
		} else {
			if (textdata != NULL) {
				delete textdata;
			}
			return draw(x, y, text, color);
		}
	}

	if (!has_draw_failed) {
		std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
		has_draw_failed = true;
	}
	return NULL;
}
TextData* BEE::Font::draw(TextData* textdata, int x, int y, std::string text) {
	return draw(textdata, x, y, text, {0, 0, 0, 255});
}
int BEE::Font::draw_fast_internal(int x, int y, std::string text, RGBA color) {
	if (is_loaded) {
		if (text.size() > 0) {
			text = string_replace(text, "\t", "    ");

			if (is_sprite) {
				int i = 0;
				int w = sprite_font->get_subimage_width();
				int h = sprite_font->get_height();
				for (char& c : text) {
					sprite_font->draw_subimage(x+(i++), y, (int)c, w, h, 0.0, {color.r, color.g, color.b, color.a}, SDL_FLIP_NONE, false);
				}
			} else {
				SDL_Surface* tmp_surface;
				tmp_surface = TTF_RenderUTF8_Solid(font, text.c_str(), {color.r, color.g, color.b, color.a}); // Fast but ugly
				if (tmp_surface == NULL) {
					std::cerr << "Failed to draw with font " << name << ": " << TTF_GetError() << "\n";
					return 1;
				}

				Sprite* tmp_sprite = new Sprite();

				tmp_sprite->load_from_surface(tmp_surface);
				tmp_sprite->draw(x, y, 0, false);

				SDL_FreeSurface(tmp_surface);
				delete tmp_sprite;
			}

			return 0;
		}

		return 1;
	}

	if (!has_draw_failed) {
		std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
		has_draw_failed = true;
	}
	return 1;
}
int BEE::Font::draw_fast(int x, int y, std::string text, RGBA color) {
	if (is_loaded) {
		std::map<int,std::string> lines = handle_newlines(text);

		if (lineskip == 0) {
			if (is_sprite) {
				lineskip = sprite_font->get_height();
			} else {
				lineskip = TTF_FontLineSkip(font);
			}
		}

		int r = 0;
		for (auto& l : lines) {
			int ri = draw_fast_internal(x, y+lineskip*l.first, l.second, color);
			if (ri > r) {
				r = ri;
			}
		}
		return r;
	}

	if (!has_draw_failed) {
		std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
		has_draw_failed = true;
	}
	return 1;
}
int BEE::Font::draw_fast(int x, int y, std::string text) {
	return draw_fast(x, y, text, {0, 0, 0, 255});
}
int BEE::Font::draw_fast(int x, int y, std::string text, rgba_t color) {
	return draw_fast(x, y, text, game->get_enum_color(color));
}

int BEE::Font::get_string_width(std::string text, int size) {
	if (is_loaded) {
		int w = 0;
		TTF_SizeUTF8(font, text.c_str(), &w, NULL);
		return w;
	}

	if (!has_draw_failed) {
		std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
		has_draw_failed = true;
	}
	return -1;
}
int BEE::Font::get_string_width(std::string text) {
	return get_string_width(text, font_size);
}
int BEE::Font::get_string_height(std::string text, int size) {
	if (is_loaded) {
		int h = 0;
		TTF_SizeUTF8(font, text.c_str(), NULL, &h);
		return h;
	}

	if (!has_draw_failed) {
		std::cerr << "Failed to draw text, font not loaded: " << name << "\n";
		has_draw_failed = true;
	}
	return -1;
}
int BEE::Font::get_string_height(std::string text) {
	return get_string_height(text, font_size);
}

#endif // _BEE_FONT
