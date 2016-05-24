/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_FONT_H
#define _BEE_FONT_H 1

#include <iostream>
#include <map>

#include "../game.hpp"

class TextData {
	public:
		std::map<int,BEE::Sprite*> sprite;
		std::string text;
		TextData() {sprite.clear();text="";};
		TextData(BEE::Sprite*, std::string);
		~TextData();
};

class BEE::Font: public Resource {
		// Add new variables to the print() debugging method
		int id = -1;
		std::string name;
		std::string font_path;
		int font_size;
		int style;
		int lineskip;

		TTF_Font* font;
		bool is_loaded = false;
		bool has_draw_failed = false;

		Sprite* sprite_font;
		bool is_sprite = false;
	public:
		Font();
		Font(std::string, std::string, int, bool);
		~Font();
		int add_to_resources(std::string);
		int reset();
		int print();

		int get_id();
		std::string get_name();
		std::string get_path();
		int get_font_size();
		int get_style();
		int get_lineskip();
		int get_lineskip_default();
		std::string get_fontname();

		int set_name(std::string);
		int set_path(std::string);
		int set_font_size(int);
		int set_style(int);
		int set_lineskip(int);

		int load();
		int free();

		TextData* draw_internal(int, int, std::string, RGBA);
		TextData* draw(int, int, std::string, RGBA);
		TextData* draw(int, int, std::string);
		TextData* draw(TextData*, int, int, std::string, RGBA);
		TextData* draw(TextData*, int, int, std::string);
		int draw_fast_internal(int, int, std::string, RGBA);
		int draw_fast(int, int, std::string, RGBA);
		int draw_fast(int, int, std::string);
		int draw_fast(int, int, std::string, bee_rgba_t);

		int get_string_width(std::string, int);
		int get_string_width(std::string);
		int get_string_height(std::string, int);
		int get_string_height(std::string);
};

#endif // _BEE_FONT_H
