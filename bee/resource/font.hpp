/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FONT_H
#define BEE_FONT_H 1

#include <string> // Include the required library headers
#include <map>
#include <list>

#include <SDL2/SDL_ttf.h> // Include the required SDL headers

#include "resource.hpp"

#include "../data/variant.hpp"

#include "../render/rgba.hpp"

namespace bee {
	// Forward declaration
	class Texture;

	/// Used to pass reusable Texture data to Font::draw()
	struct TextData {
		std::list<Texture*> textures; ///< A list of pre-rendered Textures for each line of the text
		std::string text; ///< The string of text that has been rendered

		// See bee/resource/font.cpp for function comments
		TextData();
		TextData(Texture*, const std::string&);
		~TextData();

		Texture* pop_front();
	};

	/// Used to render all text as Textures
	class Font: public Resource {
		static std::map<int,Font*> list;
		static int next_id;

		int id; ///< The unique Font identifier
		std::string name; ///< An arbitrary resource name
		std::string path; ///< The path of the TrueType font file

		int font_size; ///< The font size to render
		E_FONT_STYLE style; ///< The style of the font, see set_style() for details
		int lineskip; ///< The spacing between lines of the font

		TTF_Font* font; ///< The internal TTF font used for rendering
		bool is_loaded; ///< Whether the font file was successfully loaded
		bool has_draw_failed; ///< Whether the draw function has previously failed, this prevents continuous warning outputs

		// See bee/resource/font.cpp for function comments
		TextData* draw_internal(int, int, const std::string&, RGBA);
		int draw_fast_internal(int, int, const std::string&, RGBA);
	public:
		// See bee/resource/font.cpp for function comments
		Font();
		Font(const std::string&, const std::string&, int);
		~Font();

		static size_t get_amount();
		static Font* get(int);
		static Font* get_by_name(const std::string&);
		static Font* add(const std::string&, const std::string&, int);

		int add_to_resources();
		int reset();

		std::map<Variant,Variant> serialize() const;
		int deserialize(std::map<Variant,Variant>&);
		void print() const;

		int get_id() const;
		std::string get_name() const;
		std::string get_path() const;
		int get_font_size() const;
		E_FONT_STYLE get_style() const;
		int get_lineskip() const;
		int get_lineskip_default();
		std::string get_fontname();

		void set_name(const std::string&);
		void set_path(const std::string&);
		int set_font_size(int);
		void set_style(E_FONT_STYLE);
		void set_lineskip(int);

		int load();
		int free();

		TextData* draw(TextData*, int, int, const std::string&, RGBA);
		TextData* draw(TextData*, int, int, const std::string&);

		int draw_fast(int, int, const std::string&, RGBA);
		int draw_fast(int, int, const std::string&);

		int get_string_width(const std::string&, int) const;
		int get_string_width(const std::string&) const;
		int get_string_width() const;
		int get_string_height(const std::string&, int) const;
		int get_string_height(const std::string&) const;
		int get_string_height() const;
	};
}

#endif // BEE_FONT_H
