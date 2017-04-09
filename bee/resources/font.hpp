/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_FONT_H
#define _BEE_FONT_H 1

#include <iostream> // Include the required library headers
#include <map>

#include "../game.hpp" // Include the engine headers

class BEE::TextData { // The data struct which is used to pass reusable texture data to Font::draw()
	public:
		std::map<int,BEE::Sprite*> sprite; // A map of temporary pre-rendered sprites for each line of the text
		std::string text; // The string of text that has been rendered

		// See bee/resources/font.cpp for function comments
		TextData();
		TextData(BEE::Sprite*, std::string);
		~TextData();

		BEE::Sprite* pop_front();
};

class BEE::Font: public Resource { // The font class which is used to render all text as sprites
		int id = -1; // The id of the resource
		std::string name; // An arbitrary name for the resource
		std::string path; // The path of the TrueType font file or the sprite font image to be used for rendering
		int font_size; // The size of the font to render
		int style; // The style of the font, see Font::set_style() for more information
		int lineskip; // The spacing between lines of the font

		TTF_Font* font; // The internal TTF font used for rendering
		bool is_loaded = false; // Whether the font file was successfully loaded
		bool has_draw_failed = false; // Whether the draw function has previously failed, this prevents continuous writes to

		Sprite* sprite_font; // The optional internal sprite font used for rendering
		bool is_sprite = false; // Whether the font is a sprite font
	public:
		// See bee/resources/font.cpp for function comments
		Font();
		Font(const std::string&, const std::string&, int, bool);
		~Font();
		int add_to_resources();
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

		int set_name(const std::string&);
		int set_path(const std::string&);
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
		int get_string_height();
};

#endif // _BEE_FONT_H
