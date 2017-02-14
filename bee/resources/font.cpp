/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_FONT
#define _BEE_FONT 1

#include "font.hpp"

BEE::TextData::TextData(BEE::Sprite* new_sprite, std::string new_text) {
	sprite.insert(std::make_pair(0, new_sprite));
	text = new_text;
}
BEE::TextData::~TextData() {
	for (auto& s : sprite) {
		s.second->free();
	}
	sprite.clear();
	text = "";
}

BEE::Font::Font () {
	if (BEE::resource_list->fonts.game != nullptr) {
		game = BEE::resource_list->fonts.game;
	}
	reset();
}
BEE::Font::Font (std::string new_name, std::string new_path, int new_font_size, bool new_is_sprite) {
	reset();

	add_to_resources();
	if (id < 0) {
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add font resource: \"" + new_name + "\" from " + new_path);
		throw(-1);
	}

	set_name(new_name);
	set_path(new_path);
	set_font_size(new_font_size);
	is_sprite = new_is_sprite;
}
BEE::Font::~Font() {
	this->free();
	BEE::resource_list->fonts.remove_resource(id);
}
int BEE::Font::add_to_resources() {
	if (id < 0) { // If the resource needs to be added to the resource list
		id = BEE::resource_list->fonts.add_resource(this); // Add the resource and get the new id
	}

	// Get the list's engine pointer if it's not nullptr
	if (BEE::resource_list->fonts.game != nullptr) {
		game = BEE::resource_list->fonts.game;
	}

	return 0;
}
int BEE::Font::reset() {
	this->free();

	name = "";
	path = "";
	font_size = 16;
	style = TTF_STYLE_NORMAL;
	lineskip = 0;

	font = nullptr;
	is_loaded = false;
	has_draw_failed = false;

	sprite_font = nullptr;
	is_sprite = false;

	return 0;
}
int BEE::Font::print() {
	std::stringstream s;
	s <<
	"Font { "
	"\n	id        " << id <<
	"\n	name      " << name <<
	"\n	path      " << path <<
	"\n	font_size " << font_size <<
	"\n	style     " << style <<
	"\n	lineskip  " << lineskip <<
	"\n	font      " << font <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}
int BEE::Font::get_id() {
	return id;
}
std::string BEE::Font::get_name() {
	return name;
}
std::string BEE::Font::get_path() {
	return path;
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
	game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to get the default lineskip for \"" + name + "\" becuase it is not loaded");
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
int BEE::Font::set_path(std::string new_path) {
	path = "resources/fonts/"+new_path;
	return 0;
}
int BEE::Font::set_font_size(int new_font_size) {
	font_size = new_font_size;
	return 0;
}
int BEE::Font::set_style(int new_style) {
	if (is_loaded)	{
		style = new_style; // bitmask of TTF_STYLE_BOLD, _ITALIC, _UNDERLINE, _STRIKETHROUGH, and _NORMAL
		TTF_SetFontStyle(font, style);
		return 0;
	}

	game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to set the font style for \"" + name + "\" because it is not loaded");
	return 1;
}
int BEE::Font::set_lineskip(int new_lineskip) {
	lineskip = new_lineskip;
	return 0;
}

int BEE::Font::load() {
	if (!is_loaded) {
		if (is_sprite) {
			sprite_font = new Sprite("spr_"+get_name(), path);
			if (!sprite_font->load()) {
				game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to load the font \"" + path + "\": " + get_sdl_error());
				has_draw_failed = true;
			}

			is_loaded = true;
			has_draw_failed = false;
		} else {
			if (game->options->renderer_type != BEE_RENDERER_SDL) {
				game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Please note that TTF fast font rendering is currently broken in OpenGL mode\nThe current behavior is to draw slowly and discard the texture data");
			}

			font = TTF_OpenFont(path.c_str(), font_size);
			if (font == nullptr) {
				game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to load font \"" + path + "\": " + TTF_GetError());
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
			sprite_font = nullptr;
		} else {
			TTF_CloseFont(font);
			font = nullptr;
		}

		is_loaded = false;
	}

	return 0;
}

BEE::TextData* BEE::Font::draw_internal(int x, int y, std::string text, RGBA color) {
	if (is_loaded) {
		if (text.size() > 0) {
			text = string_replace(text, "\t", "    ");

			SDL_Surface* tmp_surface;
			tmp_surface = TTF_RenderUTF8_Blended(font, text.c_str(), {color.r, color.g, color.b, color.a}); // Slow but pretty
			if (tmp_surface == nullptr) {
				game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw with font \"" + name + "\": " + TTF_GetError());
				return nullptr;
			}

			Sprite* tmp_sprite = new Sprite();

			tmp_sprite->load_from_surface(tmp_surface);
			tmp_sprite->set_is_lightable(false);
			tmp_sprite->draw(x, y, 0, false);

			SDL_FreeSurface(tmp_surface);

			TextData* textdata = new TextData(tmp_sprite, text);

			return textdata;
		}

		return nullptr;
	}

	if (!has_draw_failed) {
		game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
		has_draw_failed = true;
	}
	return nullptr;
}
BEE::TextData* BEE::Font::draw(int x, int y, std::string text, RGBA color) {
	if (is_loaded) {
		TextData *r = nullptr, *textdata = nullptr;
		std::map<int,std::string> lines = handle_newlines(text);

		if (lineskip == 0) {
			lineskip = TTF_FontLineSkip(font);
		}

		for (auto& l : lines) {
			r = draw_internal(x, y+lineskip*l.first, l.second, color);
			if (r != nullptr) {
				if (textdata == nullptr) {
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
		game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
		has_draw_failed = true;
	}
	return nullptr;
}
BEE::TextData* BEE::Font::draw(int x, int y, std::string text) {
	return draw(x, y, text, {0, 0, 0, 255});
}
BEE::TextData* BEE::Font::draw(TextData* textdata, int x, int y, std::string text, RGBA color) {
	if (is_loaded) {
		if ((textdata != nullptr)&&(textdata->text == text)) {
			std::map<int,std::string> lines = handle_newlines(text);
			for (auto& l : lines) {
				textdata->sprite[l.first]->draw(x, y, 0, false);
			}
			return textdata;
		} else {
			if (textdata != nullptr) {
				delete textdata;
			}
			return draw(x, y, text, color);
		}
	}

	if (!has_draw_failed) {
		game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
		has_draw_failed = true;
	}
	return nullptr;
}
BEE::TextData* BEE::Font::draw(TextData* textdata, int x, int y, std::string text) {
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
					sprite_font->draw_subimage(x+(i++), y, (int)c, w, h, 0.0, {color.r, color.g, color.b, color.a}, SDL_FLIP_NONE);
				}
			} else {
				SDL_Surface* tmp_surface;
				if (game->options->renderer_type != BEE_RENDERER_SDL) { // Fast font rendering is currently broken in OpenGL
					tmp_surface = TTF_RenderUTF8_Blended(font, text.c_str(), {color.r, color.g, color.b, color.a}); // Slow but pretty
				} else {
					tmp_surface = TTF_RenderUTF8_Solid(font, text.c_str(), {color.r, color.g, color.b, color.a}); // Fast but ugly
				}
				if (tmp_surface == nullptr) {
					game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw with font \"" + name + "\": " + TTF_GetError());
					return 1;
				}

				Sprite* tmp_sprite = new Sprite();

				tmp_sprite->load_from_surface(tmp_surface);
				tmp_sprite->set_is_lightable(false);
				tmp_sprite->draw(x, y, 0, false);

				SDL_FreeSurface(tmp_surface);
				delete tmp_sprite;
			}

			return 0;
		}

		return 1;
	}

	if (!has_draw_failed) {
		game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
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
		game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
		has_draw_failed = true;
	}
	return 1;
}
int BEE::Font::draw_fast(int x, int y, std::string text) {
	return draw_fast(x, y, text, {0, 0, 0, 255});
}
int BEE::Font::draw_fast(int x, int y, std::string text, bee_rgba_t color) {
	return draw_fast(x, y, text, game->get_enum_color(color));
}

int BEE::Font::get_string_width(std::string text, int size) {
	if (is_loaded) {
		int w = 0;
		if (size == font_size) {
			TTF_SizeUTF8(font, text.c_str(), &w, nullptr);
		}
		return w;
	}

	if (!has_draw_failed) {
		game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
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
		if (size == font_size) {
			TTF_SizeUTF8(font, text.c_str(), nullptr, &h);
		}
		return h;
	}

	if (!has_draw_failed) {
		game->messenger_send({"engine", "font"}, BEE_MESSAGE_WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
		has_draw_failed = true;
	}
	return -1;
}
int BEE::Font::get_string_height(std::string text) {
	return get_string_height(text, font_size);
}
int BEE::Font::get_string_height() {
	return get_string_height("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890", font_size);
}

#endif // _BEE_FONT
