/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FONT
#define BEE_FONT 1

#include <sstream> // Include the required library headers

#include "font.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/string.hpp"
#include "../util/platform.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"

#include "../render/render.hpp"

#include "texture.hpp"

namespace bee {
	/*
	* TextData::TextData() - Construct the data struct and initialize the default values
	*/
	TextData::TextData() :
		textures(),
		text()
	{}
	/*
	* TextData::TextData() - Construct the data struct and initialize the provided values
	* @new_texture: the texture for the first line of text
	* @new_text: the text that has been rendered in the texture
	*/
	TextData::TextData(Texture* new_texture, const std::string& new_text) :
		textures(),
		text(new_text)
	{
		textures.emplace(0, new_texture); // Set the provided texture as the data for the first line
	}
	/*
	* TextData::~TextData() - Free the memory for each line's texture
	*/
	TextData::~TextData() {
		for (auto& s : textures) { // Iterate over the textures and delete each one
			delete s.second;
		}
		textures.clear(); // Clear the list
		text.clear();
	}
	/*
	* TextData::pop_front() -  Pop the front texture so it won't be deleted twice
	*/
	Texture* TextData::pop_front() {
		Texture* t = textures.begin()->second; // Store a copy of the texture pointer
		textures.erase(textures.begin()); // Erase the texture from the list
		return t; // Return the texture
	}

	std::map<int,Font*> Font::list;
	int Font::next_id = 0;

	/*
	* Font::Font() - Construct the font and set its engine pointer
	* ! This constructor should only be directly used for temporary fonts, the other constructor should be used for all other cases
	*/
	Font::Font() :
		Resource(),

		id(-1),
		name(),
		path(),
		font_size(24),
		style(TTF_STYLE_NORMAL),
		lineskip(0),

		font(nullptr),
		is_loaded(false),
		has_draw_failed(false),

		sprite_font(nullptr),
		is_sprite(false)
	{}
	/*
	* Font::Font() - Construct the font, reset all variables, add it to the font resource list, and set the new name and path
	* @new_name: the name of the font to use
	* @new_path: the path of the font file, must be either a TTF or bitmap font
	* @new_font_size: the size of the font to use
	* @new_is_sprite: whether the font is a sprite (bitmap) font
	*/
	Font::Font(const std::string& new_name, const std::string& new_path, int new_font_size, bool new_is_sprite) :
		Font() // Default initialize all variables
	{
		add_to_resources(); // Add the font to the appropriate resource list
		if (id < 0) { // If the font could not be added to the resource list, output a warning
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add font resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name); // Set the font name
		set_path(new_path); // Set the font path
		set_font_size(new_font_size); // Set the font size
		is_sprite = new_is_sprite; // Directly set whether the font is a sprite font
	}
	/*
	* Font::~Font() - Free the font data and remove it from the resource list
	*/
	Font::~Font() {
		this->free(); // Free all font data
		list.erase(id); // Remove the font from the resource list
	}

	/*
	* Font::get_amount() - Return the amount of font resources
	*/
	size_t Font::get_amount() {
		return list.size();
	}
	/*
	* Font::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Font* Font::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Font::get_by_name() - Return the font resource with the given name
	* @name: the name of the desired font
	*/
	Font* Font::get_by_name(const std::string& name) {
		for (auto& font : list) { // Iterate over the fonts in order to find the first one with the given name
			Font* f = font.second;
			if (f != nullptr) {
				if (f->get_name() == name) {
					return f; // Return the desired font on success
				}
			}
		}
		return nullptr; // Return nullptr on failure
	}
	/*
	* Font::add() - Initiliaze, load, and return a newly created font resource
	* @name: the name to initialize the font with
	* @path: the path to initialize the font with
	* @size: the font size to initialize the font with
	* @is_sprite: whether the font is a bitmap or not (i.e. ttf)
	*/
	Font* Font::add(const std::string& name, const std::string& path, int size, bool is_sprite) {
		Font* new_font = new Font(name, path, size, is_sprite);
		new_font->load();
		return new_font;
	}

	/*
	* Font::add_to_resources() - Add the font to the appropriate resource list
	*/
	int Font::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Font::reset() - Reset all resource variables for reinitialization
	*/
	int Font::reset() {
		this->free(); // Free all memory used by this resource

		// Reset all properties
		name = "";
		path = "";
		font_size = 24;
		style = TTF_STYLE_NORMAL;
		lineskip = 0;

		// Reset font data
		font = nullptr;
		is_loaded = false;
		has_draw_failed = false;

		// Reset sprite font data
		sprite_font = nullptr;
		is_sprite = false;

		return 0; // Return 0 on success
	}
	/*
	* Font::print() - Print all relevant information about the resource
	*/
	int Font::print() const {
		std::stringstream s; // Declare the output stream
		s << // Append all info to the output
		"Font { "
		"\n	id              " << id <<
		"\n	name            " << name <<
		"\n	path            " << path <<
		"\n	font_size       " << font_size <<
		"\n	style           " << style <<
		"\n	lineskip        " << lineskip <<
		"\n	font            " << font <<
		"\n	is_loaded       " << is_loaded <<
		"\n	has_draw_failed " << has_draw_failed <<
		"\n	is_sprite       " << is_sprite <<
		"\n	sprite_font     " << sprite_font <<
		"\n}\n";
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Font::get_*() - Return the requested resource information
	*/
	int Font::get_id() const {
		return id;
	}
	std::string Font::get_name() const {
		return name;
	}
	std::string Font::get_path() const {
		return path;
	}
	int Font::get_font_size() const {
		return font_size;
	}
	int Font::get_style() const {
		return style;
	}
	int Font::get_lineskip() const {
		return lineskip;
	}
	int Font::get_lineskip_default() {
		if (!is_loaded) { // If the font is not loaded, output a warning
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to get the default lineskip for \"" + name + "\" becuase it is not loaded");
			return -1; // Return -1 on failure
		}

		return TTF_FontLineSkip(font); // Return the lineskip on success
	}
	std::string Font::get_fontname() {
		std::string fontname = ""; // Create the fontname in the following format "family style", i.e. "Liberation Mono Regular"
		fontname.append(TTF_FontFaceFamilyName(font));
		fontname.append(" ");
		fontname.append(TTF_FontFaceStyleName(font));
		return fontname;
	}

	/*
	* Font::set_*() - Set the requested resource data
	*/
	int Font::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Font::set_path(const std::string& new_path) {
		if (new_path.front() == '/') {
			path = new_path.substr(1);
		} else {
			path = "resources/fonts/"+new_path; // Append the path to the font directory if no root
		}
		return 0;
	}
	int Font::set_font_size(int new_font_size) {
		font_size = new_font_size;
		return 0;
	}
	/*
	* Font::set_style() - Set the font style to use with the font
	* @new_style: the desired style, a bitmask of TTF_STYLE_BOLD, _ITALIC, _UNDERLINE, _STRIKETHROUGH, and _NORMAL
	*/
	int Font::set_style(int new_style) {
		if (!is_loaded) { // Do not attempt to set the style if the font has not been loaded
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to set the font style for \"" + name + "\" because it is not loaded");
			return 1; // Return 1 on error
		}

		style = new_style; // Store the style
		TTF_SetFontStyle(font, style); // Set the style of the loaded font
		return 0; // Return 0 on success
	}
	int Font::set_lineskip(int new_lineskip) {
		lineskip = new_lineskip;
		return 0;
	}

	/*
	* Font::load() - Load the font from its given filename
	*/
	int Font::load() {
		if (is_loaded) { // If the font has already been loaded, output a warning
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to load font \"" + name + "\" from because it has already been loaded");
			return 1; // Return 1 when already loaded
		}

		if (get_options().is_headless) {
			return 2; // Return 2 when in headless mode
		}

		if (is_sprite) { // If the font is a sprite font, load it appropriately
			sprite_font = new Texture("spr_"+get_name(), path); // Create and load a sprite for the font
			if (!sprite_font->load()) { // If the sprite fails to load, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to load the font \"" + path + "\": " + get_sdl_error());
				has_draw_failed = true;
				return 3; // Return 3 on loading failure
			}

			// Set the loaded booleans
			is_loaded = true;
			has_draw_failed = false;
		} else { // Otherwise load the sprite's TTF file
			if ((get_options().renderer_type == E_RENDERER::OPENGL3)||(get_options().renderer_type == E_RENDERER::OPENGL4)) { // If the engine is rendering in OpenGL mode, output a warning about fast font drawing
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Please note that TTF fast font rendering is currently broken in OpenGL mode\nThe current behavior is to draw slowly and discard the texture data");
			}

			font = TTF_OpenFont(path.c_str(), font_size); // Open the TTF file with the desired font size
			if (font == nullptr) { // If the font failed to load, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to load font \"" + path + "\": " + TTF_GetError());
				has_draw_failed = true;
				return 4; // Return 4 on loading failure
			}

			// Set the loaded booleans
			is_loaded = true;
			has_draw_failed = false;
		}

		return 0; // Return 0 on success
	}
	/*
	* Font::free() - Free the font textures and other data
	*/
	int Font::free() {
		if (is_loaded) { // Do not attempt to free the data if the font has not been loaded
			if (is_sprite) { // If the font is a sprite font, free it appropriately
				// Delete the sprite for the font
				delete sprite_font;
				sprite_font = nullptr;
			} else {
				// Delete the TTF font
				TTF_CloseFont(font);
				font = nullptr;
			}

			// Set the loaded booleans
			is_loaded = false;
			has_draw_failed = false;
		}

		return 0; // Return 0 on success
	}

	/*
	* Font::draw_internal() - Draw the given text with the given attributes
	* @x: the x-coordinate to draw the text at
	* @y: the y-coordinate to draw the text at
	* @text: the string to draw
	* @color: the color to draw the text in
	*/
	TextData* Font::draw_internal(int x, int y, const std::string& text, RGBA color) {
		if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
				has_draw_failed = true; // Set the draw failure boolean
			}
			return nullptr; // Return nullptr when not loaded
		}

		if (text.empty()) {
			return nullptr; // Return nullptr when no text was provided
		}

		std::string t = string_replace(text, "\t", "    "); // Replace all tabs with 4 spaces for proper rendering

		// Render the text to a temporary surface
		SDL_Surface* tmp_surface = TTF_RenderUTF8_Blended(font, t.c_str(), {color.r, color.g, color.b, color.a}); // Use the slow but pretty TTF rendering mode
		if (tmp_surface == nullptr) { // If the text failed to render, output a warning
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw with font \"" + name + "\": " + TTF_GetError());
			return nullptr; // Return nullptr when rendering failed
		}

		// Create a temporary sprite to draw the rendered text to the screen
		Texture* tmp_texture = new Texture();
		tmp_texture->load_from_surface(tmp_surface); // Load the rendered text into the texture
		SDL_FreeSurface(tmp_surface); // Free the temporary surface

		tmp_texture->draw(x, y, 0); // Draw the text
		render::render_textures();

		TextData* textdata = new TextData(tmp_texture, t); // Store the temporary texture in a TextData struct

		return textdata; // Return the textdata on success
	}
	/*
	* Font::draw() - Draw the given text with the given attributes
	* @x: the x-coordinate to draw the text at
	* @y: the y-coordinate to draw the text at
	* @text: the string to draw
	* @color: the color to draw the text in
	*/
	TextData* Font::draw(int x, int y, const std::string& text, RGBA color) {
		if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
				has_draw_failed = true; // Set the draw failure boolean
			}
			return nullptr; // Return nullptr when not loaded
		}

		if (lineskip == 0) { // If the lineskip hasn't been set yet, get it from the loaded font
			lineskip = TTF_FontLineSkip(font);
		}

		TextData *textdata = nullptr, *r = nullptr; // Create a pointer for the entire textdata and a temporary one for each line
		std::map<int,std::string> lines = handle_newlines(text); // Separate the text by newline
		for (auto& l : lines) { // Iterate over the lines and draw each one
			r = draw_internal(x, y+lineskip*l.first, l.second, color); // Draw the line at the appropriate coordinates

			if (r != nullptr) { // If the line was successfully drawn, append its textdata
				if (textdata == nullptr) { // If it's the first line, set it as the entire textdata
					textdata = r;
				} else { // Otherwise append its data to the existing textdata
					textdata->textures.emplace(textdata->textures.size(), r->pop_front());
					delete r; // Free the temporary data
				}
			}
		}
		return textdata; // Return the textdata on success
	}
	/*
	* Font::draw() - Draw the given text at the given coordinates
	* ! When the function is called without a color, simply draw it in black
	* @x: the x-coordinate to draw the text at
	* @y: the y-coordinate to draw the text at
	* @text: the string to draw
	*/
	TextData* Font::draw(int x, int y, const std::string& text) {
		return draw(x, y, text, {0, 0, 0, 255}); // Return the result of drawing the text in black
	}
	/*
	* Font::draw() - Draw the given text with the given attributes
	* ! Note that the user is only responsible for freeing the returned TextData after the last time that they draw their text
	* ! This allows the following syntax:
		td = font->draw(td, x, y, text, color);
	* @textdata: the textdata to draw if the given text matches the prerendered text
	* @x: the x-coordinate to draw the text at
	* @y: the y-coordinate to draw the text at
	* @text: the string to draw
	* @color: the color to draw the text in
	*/
	TextData* Font::draw(TextData* textdata, int x, int y, const std::string& text, RGBA color) {
		if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
				has_draw_failed = true; // Set the draw failure boolean
			}
			return nullptr; // Return nullptr when not loaded
		}

		if ((textdata != nullptr)&&(textdata->text == text)) { // If the prerendered text matches the given text, draw it as-is
			std::vector<std::string> lines = splitv(text, '\n', false); // Separate the text by newline
			size_t i = 0;
			for (auto& l : lines) { // Iterate over the lines and draw each one
				if (!l.empty()) {
					textdata->textures[i]->draw(x, y, 0);
				}
				++i;
			}
			return textdata; // Return the textdata on success
		} else { // Otherwise, free the given textdata and redraw the text
			if (textdata != nullptr) {
				delete textdata; // Free the old textdata
			}
			return draw(x, y, text, color); // Return the result of redrawing the text
		}
	}
	/*
	* Font::draw() - Draw the given text at the given coordinates
	* ! When the function is called without a color, simply draw it in black
	* @textdata: the textdata to draw if the given text matches the prerendered text
	* @x: the x-coordinate to draw the text at
	* @y: the y-coordinate to draw the text at
	* @text: the string to draw
	*/
	TextData* Font::draw(TextData* textdata, int x, int y, const std::string& text) {
		return draw(textdata, x, y, text, {0, 0, 0, 255}); // Return the result of drawing the text in black
	}

	/*
	* Font::draw_fast_internal() - Draw the given text with the given attributes without storing the rendered text
	* @x: the x-coordinate to draw the text at
	* @y: the y-coordinate to draw the text at
	* @text: the string to draw
	* @color: the color to draw the text in
	*/
	int Font::draw_fast_internal(int x, int y, const std::string& text, RGBA color) {
		if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
				has_draw_failed = true; // Set the draw failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (text.empty()) {
			return 0; // Return 0 when no text was provided
		}

		std::string t = string_replace(text, "\t", "    "); // Replace all tabs with 4 spaces for proper rendering

		if (is_sprite) { // If the font is a sprite font, draw it appropriately
			int i = 0;
			int w = sprite_font->get_subimage_width();
			int h = sprite_font->get_height();
			for (char& c : t) {
				sprite_font->draw_subimage(x+(i++), y, static_cast<int>(c), w, h, 0.0, {color.r, color.g, color.b, color.a});
			}
		} else { // Otherwise, draw the font normally
			// Render the text to a temporary surface
			SDL_Surface* tmp_surface;
			tmp_surface = TTF_RenderUTF8_Blended(font, t.c_str(), {color.r, color.g, color.b, color.a}); // Use the slow but pretty TTF rendering mode
			//tmp_surface = TTF_RenderUTF8_Solid(font, t.c_str(), {color.r, color.g, color.b, color.a}); // Use the fast but ugly TTF rendering mode
			if (tmp_surface == nullptr) { // If the text failed to render, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw with font \"" + name + "\": " + TTF_GetError());
				return 2; // Return 2 on rendering error
			}

			// Create a temporary sprite to draw the rendered text to the screen
			Texture* tmp_texture = new Texture();
			tmp_texture->load_from_surface(tmp_surface); // Load the rendered text into the texture
			SDL_FreeSurface(tmp_surface); // Free the temporary surface

			tmp_texture->draw(x, y, 0); // Draw the text
			render::render_textures();

			delete tmp_texture; // Free the temporary texture
		}

		return 0; // Return 0 on success
	}
	/*
	* Font::draw_fast() - Draw the given text with the given attributes without storing the rendered text
	* @x: the x-coordinate to draw the text at
	* @y: the y-coordinate to draw the text at
	* @text: the string to draw
	* @color: the color to draw the text in
	*/
	int Font::draw_fast(int x, int y, const std::string& text, RGBA color) {
		if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
				has_draw_failed = true; // Set the draw failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (lineskip == 0) { // If the lineskip hasn't been set yet, get it from the loaded font
			if (is_sprite) { // If the font is a sprite font, get the lineskip appropriately
				lineskip = sprite_font->get_height();
			} else { // Otherwise, get if from the loaded TTF font
				lineskip = TTF_FontLineSkip(font);
			}
		}

		std::map<int,std::string> lines = handle_newlines(text); // Split the text by newline
		int r = 0; // Define a return value
		for (auto& l : lines) { // Iterate over the lines and draw each one
			int ri = draw_fast_internal(x, y+lineskip*l.first, l.second, color); // Draw the line at the appropriate coordinates
			if (ri > r) { // Only store the return value if it's higher than the previous value
				r = ri;
			}
		}
		return r; // Return the highest error value from draw_fast_internal() above
	}
	/*
	* Font::draw_fast() - Draw the given text with the given attributes without storing the rendered text
	* ! When the function is called without a color, simply draw it in black
	* @x: the x-coordinate to draw the text at
	* @y: the y-coordinate to draw the text at
	* @text: the string to draw
	*/
	int Font::draw_fast(int x, int y, const std::string& text) {
		return draw_fast(x, y, text, {0, 0, 0, 255}); // Return the result of drawing the text in black
	}

	/*
	* Font::get_string_width() - Return the width of the given text in the given font size
	* @text: the string to get the width of
	* @size: the font size to use when getting the string width
	*/
	int Font::get_string_width(const std::string& text, int size) const {
		if ((!is_loaded)||(is_sprite)) { // Do not attempt to get the string width if the font has not been loaded
			return -1; // Return -1 when not loaded
		}

		int w = 0; // Declare a temporary variable for the width
		if (size == font_size) { // If the desired size is the same as the currently loaded size, fetch the width appropriately
			std::vector<std::string> lines = splitv(text, '\n', false); // Separate the text by newline
			int w0 = 0;
			for (auto& l : lines) {
				TTF_SizeUTF8(font, l.c_str(), &w0, nullptr);
				if (w0 > w) {
					w = w0;
				}
			}
		} else { // Otherwise, load a temporary font
			TTF_Font* tmp_font = TTF_OpenFont(path.c_str(), size); // Open the same TTF file with the desired font size
			if (tmp_font == nullptr) { // If the font failed to load, output a warning
				return -2; // Return -2 when font loading failed
			}

			std::vector<std::string> lines = splitv(text, '\n', false); // Separate the text by newline
			int w0 = 0;
			for (auto& l : lines) {
				TTF_SizeUTF8(tmp_font, l.c_str(), &w0, nullptr); // Get the temporary width
				if (w0 > w) {
					w = w0;
				}
			}

			TTF_CloseFont(tmp_font); // Close the font after getting the width
		}

		return w; // Return the width on success
	}
	/*
	* Font::get_string_width() - Return the width of the given text
	* ! When the function is called without a font size, simply call it with the font's loaded size
	* @text: the string to get the width of
	*/
	int Font::get_string_width(const std::string& text) const {
		return get_string_width(text, font_size); // Return the width in the current font size
	}
	/*
	* Font::get_string_width() - Return the width of a character
	* ! When the function is called without any arguments, simply call it with the conventionally largest character
	*/
	int Font::get_string_width() const {
		return get_string_width("W"); // Return the width of a large character in the current font size
	}
	/*
	* Font::get_string_height() - Return the height of the given text in the given font size
	* @text: the string to get the height of
	* @size: the font size to use when getting the string height
	*/
	int Font::get_string_height(const std::string& text, int size) const {
		if ((!is_loaded)||(is_sprite)) { // Do not attempt to get the string height if the font has not been loaded
			return -1; // Return -1 when not loaded
		}

		int h = 0; // Declare a temporary variable for the height
		if (size == font_size) {
			std::vector<std::string> lines = splitv(text, '\n', false); // Separate the text by newline
			int h0 = 0;
			for (auto& l : lines) {
				TTF_SizeUTF8(font, l.c_str(), nullptr, &h0);
				h += h0 + lineskip;
			}
		} else { // Otherwise, load a temporary font
			TTF_Font* tmp_font = TTF_OpenFont(path.c_str(), size); // Open the same TTF file with the desired font size
			if (tmp_font == nullptr) { // If the font failed to load, output a warning
				return -2; // Return -2 when font loading failed
			}

			std::vector<std::string> lines = splitv(text, '\n', false); // Separate the text by newline
			int h0 = 0;
			for (auto& l : lines) {
				TTF_SizeUTF8(tmp_font, l.c_str(), nullptr, &h0); // Get the temporary height
				h += h0 + lineskip;
			}

			TTF_CloseFont(tmp_font); // Close the font after getting the height
		}

		return h; // Return the height on success
	}
	/*
	* Font::get_string_height() - Return the height of the given text
	* ! When the function is called without a font size, simply call it with the font's loaded size
	* @text: the string to get the height of
	*/
	int Font::get_string_height(const std::string& text) const {
		return get_string_height(text, font_size); // Return the height in the current font size
	}
	/*
	* Font::get_string_height() - Return the height of a generic English string
	* ! When the function is called without any arguments, simply call it with the a generic English string and the font's loaded size
	*/
	int Font::get_string_height() const {
		return get_string_height("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890`~!@#$%^&*(),./;'[]\\-='<>?:\"{}|_+", font_size); // Return the height of most general characters in the current font size
	}
}

#endif // BEE_FONT
