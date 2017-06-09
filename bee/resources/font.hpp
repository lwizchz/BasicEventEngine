/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FONT_H
#define BEE_FONT_H 1

#include <map> // Include the required library headers

#include <SDL2/SDL_ttf.h>

#include "../resources.hpp"

#include "../render/rgba.hpp"

namespace bee {
	class Sprite;

	struct TextData { // The data struct which is used to pass reusable texture data to Font::draw()
		std::map<int,Sprite*> sprites; // A map of temporary pre-rendered sprites for each line of the text
		std::string text; // The string of text that has been rendered

		// See bee/resources/font.cpp for function comments
		TextData();
		TextData(Sprite*, const std::string&);
		~TextData();

		Sprite* pop_front();
	};

	class Font: public Resource { // The font class is used to render all text as sprites
			int id; // The id of the resource
			std::string name; // An arbitrary name for the resource
			std::string path; // The path of the TrueType font file or the sprite font image to be used for rendering
			int font_size; // The size of the font to render
			int style; // The style of the font, see Font::set_style() for more information
			int lineskip; // The spacing between lines of the font

			TTF_Font* font; // The internal TTF font used for rendering
			bool is_loaded; // Whether the font file was successfully loaded
			bool has_draw_failed; // Whether the draw function has previously failed, this prevents continuous writes to

			Sprite* sprite_font; // The optional internal sprite font used for rendering
			bool is_sprite; // Whether the font is a sprite font
		public:
			// See bee/resources/font.cpp for function comments
			Font();
			Font(const std::string&, const std::string&, int, bool);
			~Font();
			int add_to_resources();
			int reset();
			int print() const;

			int get_id() const;
			std::string get_name() const;
			std::string get_path() const;
			int get_font_size() const;
			int get_style() const;
			int get_lineskip() const;
			int get_lineskip_default();
			std::string get_fontname();

			int set_name(const std::string&);
			int set_path(const std::string&);
			int set_font_size(int);
			int set_style(int);
			int set_lineskip(int);

			int load();
			int free();

			TextData* draw_internal(int, int, const std::string&, RGBA);
			TextData* draw(int, int, const std::string&, RGBA);
			TextData* draw(int, int, const std::string&);
			TextData* draw(TextData*, int, int, const std::string&, RGBA);
			TextData* draw(TextData*, int, int, const std::string&);

			int draw_fast_internal(int, int, const std::string&, RGBA);
			int draw_fast(int, int, const std::string&, RGBA);
			int draw_fast(int, int, const std::string&);

			int get_string_width(const std::string&, int) const;
			int get_string_width(const std::string&) const;
			int get_string_height(const std::string&, int) const;
			int get_string_height(const std::string&) const;
			int get_string_height() const;
	};
}

#endif // BEE_FONT_H
