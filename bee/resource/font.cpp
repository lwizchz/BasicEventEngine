/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARRANTY.
* See LICENSE for more details.
*/

#ifndef BEE_FONT
#define BEE_FONT 1

#include "font.hpp" // Include the class resource header

#include "../util/string.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../fs/fs.hpp"

#include "../render/render.hpp"

#include "texture.hpp"

namespace bee {
	/**
	* Default construct the data struct.
	*/
	TextData::TextData() :
		textures(),
		text()
	{}
	/**
	* Construct the data struct with a pre-rendered Texture for its text.
	* @param texture the Texture for the first line of text
	* @param _text the text that has been rendered in the Texture
	*/
	TextData::TextData(Texture* texture, const std::string& _text) :
		textures(),
		text(_text)
	{
		textures.emplace_back(texture); // Set the provided Texture for the first line of text
	}
	/**
	* Free the memory for each line's Texture.
	*/
	TextData::~TextData() {
		for (auto& s : textures) { // Iterate over the Textures and delete each one
			delete s;
		}
		textures.clear();
		text.clear();
	}
	/**
	* Pop the first Texture so it can be combined with another TextData.
	*/
	Texture* TextData::pop_front() {
		Texture* t = textures.front();
		textures.pop_front();
		return t;
	}

	std::map<size_t,Font*> Font::list;
	size_t Font::next_id = 0;

	/**
	* Default construct the Font.
	* @note This constructor should only be used for temporary Fonts, the other constructor should be used for all other cases.
	*/
	Font::Font() :
		Resource(),

		id(-1),
		name(),
		path(),

		font_size(24),
		style(E_FONT_STYLE::NORMAL),
		lineskip(0),

		font(nullptr),
		is_loaded(false),
		has_draw_failed(false)
	{}
	/**
	* Construct the Font, add it to the Font resource list, and set the new name, path, and size.
	* @param _name the name of the Font to use
	* @param _path the path of the font file, must be either a TTF or OTF font
	* @param _font_size the size of the Font to use
	*
	* @throws int(-1) Failed to initialize Resource
	*/
	Font::Font(const std::string& _name, const std::string& _path, int _font_size) :
		Font() // Default initialize all variables
	{
		if (add_to_resources() == static_cast<size_t>(-1)) { // Attempt to add the Font to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add Font resource: \"" + _name + "\" from " + _path);
			throw -1;
		}

		set_name(_name);
		set_path(_path);
		set_font_size(_font_size); // Set the Font size
	}
	/**
	* Free the Font data and remove it from the resource list.
	*/
	Font::~Font() {
		this->free();
		list.erase(id);
	}

	/**
	* @returns the number of Font resources
	*/
	size_t Font::get_amount() {
		return list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id or nullptr if not found
	*/
	Font* Font::get(size_t id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Font
	*
	* @returns the Font resource with the given name or nullptr if not found
	*/
	Font* Font::get_by_name(const std::string& name) {
		for (auto& font : list) { // Iterate over the Fonts in order to find the first one with the given name
			Font* f = font.second;
			if (f != nullptr) {
				if (f->get_name() == name) {
					return f; // Return the desired Font on success
				}
			}
		}
		return nullptr;
	}
	/**
	* Initiliaze, load, and return a newly created Font resource.
	* @param name the name to initialize the Font with
	* @param path the path to initialize the Font with
	* @param size the font size to initialize the Font with
	*
	* @returns the newly loaded Font
	*/
	Font* Font::add(const std::string& name, const std::string& path, int size) {
		Font* new_font = new Font(name, path, size);
		new_font->load();
		return new_font;
	}

	/**
	* Add the Font to the appropriate resource list.
	*
	* @returns the Font id
	*/
	size_t Font::add_to_resources() {
		if (id == static_cast<size_t>(-1)) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource with its new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
	*/
	int Font::reset() {
		this->free(); // Free all memory used by this resource

		// Reset all properties
		name = "";
		path = "";
		font_size = 24;
		style = E_FONT_STYLE::NORMAL;
		lineskip = 0;

		// Reset font data
		font = nullptr;
		is_loaded = false;
		has_draw_failed = false;

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Font
	*/
	std::map<Variant,Variant> Font::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = static_cast<int>(id);
		info["name"] = name;
		info["path"] = path;

		info["font_size"] = font_size;
		info["style"] = static_cast<int>(style);
		info["lineskip"] = lineskip;

		info["font"] = font;
		info["is_loaded"] = is_loaded;
		info["has_draw_failed"] = has_draw_failed;

		return info;
	}
	/**
	* Restore the Font from serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	* @retval 1 failed to load the Font
	*/
	int Font::deserialize(std::map<Variant,Variant>& m) {
		this->free();

		id = m["id"].i;
		name = m["name"].s;
		path = m["path"].s;

		font_size = m["font_size"].i;
		style = static_cast<E_FONT_STYLE>(m["style"].i);
		lineskip = m["lineskip"].i;

		font = nullptr;
		is_loaded = false;
		has_draw_failed = m["has_draw_failed"].i;

		if ((m["is_loaded"].i)&&(load())) {
			return 1;
		}

		return 0;
	}
	/**
	* Print all relevant information about the resource.
	*/
	void Font::print() const {
		Variant m (serialize());
		messenger::send({"engine", "font"}, E_MESSAGE::INFO, "Font " + m.to_str(true));
	}

	size_t Font::get_id() const {
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
	E_FONT_STYLE Font::get_style() const {
		return style;
	}
	int Font::get_lineskip() const {
		return lineskip;
	}
	/**
	* @returns the Font's default lineskip
	* @retval -1 failed to get lineskip since it's not loaded
	*/
	int Font::get_lineskip_default() const {
		if (!is_loaded) { // If the Font is not loaded, output a warning
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to get the default lineskip for \"" + name + "\" becuase it is not loaded");
			return -1;
		}

		return TTF_FontLineSkip(font);
	}
	/**
	* @returns the font name from the TTF file
	* @retval "" failed to get the name since it's not loaded
	*/
	std::string Font::get_fontname() const {
		if (!is_loaded) {
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to get the font name for \"" + name + "\" becuase it is not loaded");
			return "";
		}

		std::string fontname = ""; // Create the fontname in the following format "family style", i.e. "Liberation Mono Regular"
		fontname.append(TTF_FontFaceFamilyName(font));
		fontname.append(" ");
		fontname.append(TTF_FontFaceStyleName(font));

		return fontname;
	}
	bool Font::get_is_loaded() const {
		return is_loaded;
	}

	void Font::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Fonts resource directory.
	*/
	void Font::set_path(const std::string& _path) {
		if (_path.empty()) {
			path.clear();
		} else if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the Font directory if not root
			path = "resources/fonts/"+_path;
		}
	}
	/**
	* Set the font size.
	* @param _font_size the desired Font size
	*
	* @retval 0 success
	* @retval >1 failed to reload the Font
	*/
	int Font::set_font_size(int _font_size) {
		font_size = _font_size;

		if (!is_loaded) {
			return 0;
		}

		// Reload the Font
		free();
		return load();
	}
	/**
	* Set the font style to use.
	* @param _style the desired style, a bitmask of TTF_STYLE_BOLD, _ITALIC, _UNDERLINE, _STRIKETHROUGH, and _NORMAL
	*/
	void Font::set_style(E_FONT_STYLE _style) {
		style = _style; // Store the style

		if (is_loaded) { // Only set the style if the Font is loaded
			TTF_SetFontStyle(font, static_cast<int>(style));
		}
	}
	void Font::set_lineskip(int _lineskip) {
		lineskip = _lineskip;
	}

	/**
	* Load the Font from its path.
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load since the engine is in headless mode
	* @retval 3 failed to load the font file
	*/
	int Font::load() {
		if (is_loaded) { // If the Font has already been loaded, output a warning
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to load Font \"" + name + "\" because it has already been loaded");
			return 1;
		}

		if (get_option("is_headless").i) {
			return 2;
		}

		if (
			(get_option("renderer_type").i == static_cast<int>(E_RENDERER::OPENGL3))
			||(get_option("renderer_type").i == static_cast<int>(E_RENDERER::OPENGL4))
		) { // If the engine is rendering in OpenGL mode, output a warning about fast font drawing
			if (this == Font::list.at(0)) { // Only output the warning for the first loaded Font
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "TTF fast font rendering is currently broken in OpenGL mode\nThe current behavior is to draw slowly and discard the texture data");
			}
		}

		font = TTF_OpenFontRW(fs::get_file(path).get_rwops(), true, font_size); // Open the TTF file with the desired font size
		if (font == nullptr) { // If the Font failed to load, output a warning
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to load Font \"" + name + "\" from \"" + path + "\": " + util::get_sdl_error());
			has_draw_failed = true;
			return 3;
		}

		TTF_SetFontStyle(font, static_cast<int>(style));

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;

		return 0;
	}
	/**
	* Free the Font.
	*
	* @retval 0 success
	*/
	int Font::free() {
		if (!is_loaded) { // Do not attempt to free the data if the Font has not been loaded
			return 0;
		}

		// Delete the TTF font
		TTF_CloseFont(font);
		font = nullptr;

		// Set the loaded booleans
		is_loaded = false;
		has_draw_failed = false;

		return 0;
	}

	/**
	* Draw the given text with the given attributes.
	* @param x the x-coordinate to draw the text at
	* @param y the y-coordinate to draw the text at
	* @param text the string to draw
	* @param color the color with which to draw the text
	*
	* @returns the rendered Texture data as a TextData
	*/
	TextData* Font::draw_internal(int x, int y, const std::string& text, RGBA color) {
		if (text.empty()) {
			return nullptr;
		}

		std::string t = util::string::replace(text, "\t", "    "); // Replace all tabs with 4 spaces for proper rendering

		// Render the text to a temporary surface
		SDL_Surface* tmp_surface = TTF_RenderUTF8_Blended(font, t.c_str(), {color.r, color.g, color.b, color.a}); // Use the slow but pretty TTF rendering mode
		if (tmp_surface == nullptr) { // If the text failed to render, output a warning
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw with Font \"" + name + "\": " + util::get_sdl_error());
			return nullptr;
		}

		// Create a temporary texture to draw the rendered text to the screen
		Texture* tmp_texture = new Texture();
		tmp_texture->load_from_surface(tmp_surface); // Load the rendered text into the texture
		SDL_FreeSurface(tmp_surface);

		tmp_texture->draw(x, y, 0); // Draw the text

		TextData* textdata = new TextData(tmp_texture, t); // Store the temporary texture in a TextData struct

		return textdata;
	}
	/**
	* Draw the given text with the given attributes.
	* @note The user is responsible for freeing the returned TextData after the last time they draw their text.
	* @note This allows the following syntax:
	* @code
	*     td = font->draw(td, x, y, text, color);
	* @endcode
	*
	* @param textdata the TextData to draw if the given text matches the prerendered text
	* @param x the x-coordinate to draw the text at
	* @param y the y-coordinate to draw the text at
	* @param text the string to draw
	* @param color the color to draw the text in
	*
	* @returns the rendered Texture data as a TextData
	*/
	TextData* Font::draw(TextData* textdata, int x, int y, const std::string& text, RGBA color) {
		if (!is_loaded) { // Do not attempt to draw the text if the Font has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
				has_draw_failed = true;
			}
			return nullptr;
		}

		if (lineskip == 0) { // If the lineskip hasn't been set yet, get it from the loaded Font
			lineskip = TTF_FontLineSkip(font);
		}

		std::vector<std::string> lines = util::splitv(text, '\n', false); // Separate the text by newline

		if ((textdata != nullptr)&&(textdata->text == text)) { // If the prerendered text matches the given text, draw it as-is
			size_t i = 0;
			for (auto& t : textdata->textures) { // Iterate over the lines and draw each one
				if (!lines[i].empty()) {
					t->draw(x, y+lineskip*i, 0);
				}
				++i;
			}
		} else { // Otherwise, free the given textdata and redraw the text
			if (textdata != nullptr) {
				delete textdata; // Free the old textdata
				textdata = nullptr;
			}

			TextData *r = nullptr; // Create a temporary TextData for each line
			size_t i = 0;
			for (auto& l : lines) { // Iterate over the lines and draw each one
				r = draw_internal(x, y+lineskip*i, l, color); // Draw the line at the appropriate coordinates

				if (r != nullptr) { // If the line was successfully drawn, append its TextData
					if (textdata == nullptr) { // If it's the first line, set it as the entire TextData
						textdata = r;
					} else { // Otherwise append its data to the existing TextData
						textdata->text += "\n" + r->text;
						textdata->textures.emplace_back(r->pop_front());
						delete r; // Free the temporary data
					}
				}

				++i;
			}
		}

		return textdata;
	}
	/**
	* Draw the given text at the given coordinates.
	* @note When the function is called without a color, let it be black.
	* @param textdata the TextData to draw if the given text matches the prerendered text
	* @param x the x-coordinate to draw the text at
	* @param y the y-coordinate to draw the text at
	* @param text the string to draw
	*
	* @returns the rendered Texture data as a TextData
	*/
	TextData* Font::draw(TextData* textdata, int x, int y, const std::string& text) {
		return draw(textdata, x, y, text, {0, 0, 0, 255});
	}

	/**
	* Draw the given text with the given attributes without storing the rendered text.
	* @param x the x-coordinate to draw the text at
	* @param y the y-coordinate to draw the text at
	* @param text the string to draw
	* @param color the color to draw the text in
	*
	* @retval 0 success
	* @retval 1 failed to render TTF text
	*/
	int Font::draw_fast_internal(int x, int y, const std::string& text, RGBA color) {
		if (text.empty()) {
			return 0;
		}

		std::string t = util::string::replace(text, "\t", "    "); // Replace all tabs with 4 spaces for proper rendering

		// Render the text to a temporary surface
		SDL_Surface* tmp_surface;
		tmp_surface = TTF_RenderUTF8_Blended(font, t.c_str(), {color.r, color.g, color.b, color.a}); // Use the slow but pretty TTF rendering mode
		//tmp_surface = TTF_RenderUTF8_Solid(font, t.c_str(), {color.r, color.g, color.b, color.a}); // Use the fast but ugly TTF rendering mode
		if (tmp_surface == nullptr) { // If the text failed to render, output a warning
			messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw with Font \"" + name + "\": " + util::get_sdl_error());
			return 1;
		}

		// Create a temporary Texture to draw the rendered text to the screen
		Texture* tmp_texture = new Texture();
		tmp_texture->load_from_surface(tmp_surface); // Load the rendered text into the Texture
		SDL_FreeSurface(tmp_surface);

		tmp_texture->draw(x, y, 0); // Draw the text
		render::render_textures(); // Render the Texture before freeing it

		delete tmp_texture; // Free the temporary Texture

		return 0;
	}
	/**
	* Draw the given text with the given attributes without storing the rendered text.
	* @param x the x-coordinate to draw the text at
	* @param y the y-coordinate to draw the text at
	* @param text the string to draw
	* @param color the color to draw the text in
	*
	* @retval 0 success
	* @retval 1 failed to draw since it's not loaded
	* @retval 2 failed to draw otherwise
	*/
	int Font::draw_fast(int x, int y, const std::string& text, RGBA color) {
		if (!is_loaded) { // Do not attempt to draw the text if the Font has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "font"}, E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
				has_draw_failed = true;
			}
			return 1;
		}

		if (lineskip == 0) { // If the lineskip hasn't been set yet, get it from the loaded Font
			lineskip = TTF_FontLineSkip(font);
		}

		std::vector<std::string> lines = util::splitv(text, '\n', false); // Split the text by newline
		int r = 0; // Define a return value
		size_t i = 0;
		for (auto& l : lines) { // Iterate over the lines and draw each one
			r += draw_fast_internal(x, y+lineskip*i, l, color); // Draw the line at the appropriate coordinates
			++i;
		}
		return !!r;
	}
	/**
	* Draw the given text with the given attributes without storing the rendered text.
	* @note When the function is called without a color, let it be black.
	* @param x the x-coordinate to draw the text at
	* @param y the y-coordinate to draw the text at
	* @param text the string to draw
	*
	* @retval 0 success
	* @retval >0 failed to draw
	* @see draw_fast(int, int, const std::string&, RGBA) for details
	*/
	int Font::draw_fast(int x, int y, const std::string& text) {
		return draw_fast(x, y, text, {0, 0, 0, 255}); // Return the result of drawing the text in black
	}

	/**
	* @param text the string to get the width of
	* @param size the font size to use when getting the string width
	*
	* @returns the width of the given text in the given font size
	* @retval -1 failed to load the temporary font
	*/
	int Font::get_string_width(const std::string& text, int size) const {
		TTF_Font* tmp_font = TTF_OpenFont(path.c_str(), size); // Open the TTF file with the desired font size
		if (tmp_font == nullptr) {
			return -1;
		}

		std::vector<std::string> lines = util::splitv(text, '\n', false); // Separate the text by newline
		int w = 0, w0 = 0;
		for (auto& l : lines) {
			TTF_SizeUTF8(tmp_font, l.c_str(), &w0, nullptr); // Get the temporary width
			if (w0 > w) { // Store the maximum line width
				w = w0;
			}
		}

		TTF_CloseFont(tmp_font); // Close the temp font

		return w;
	}
	/**
	* @note When the function is called without a font size, let it be the Font's loaded size.
	* @param text the string to get the width of
	*
	* @returns the width of the given text
	* @see get_string_width(const std::string&, int) const for details
	*/
	int Font::get_string_width(const std::string& text) const {
		return get_string_width(text, font_size);
	}
	/**
	* @note When the function is called with no arguments, let the string be "W" and the font size be the loaded size.
	*
	* @returns the width of a large character
	* @see get_string_width(const std::string&, int) const for details
	*/
	int Font::get_string_width() const {
		return get_string_width("W");
	}
	/**
	* @param text the string to get the height of
	* @param size the font size to use when getting the string height
	*
	* @returns the height of the given text in the given font size
	* @retval -1 failed to load the temporary font
	*/
	int Font::get_string_height(const std::string& text, int size) const {
		TTF_Font* tmp_font = TTF_OpenFont(path.c_str(), size); // Open the TTF file with the desired font size
		if (tmp_font == nullptr) {
			return -1;
		}

		std::vector<std::string> lines = util::splitv(text, '\n', false); // Separate the text by newline
		int h = 0, h0 = 0;
		for (auto& l : lines) {
			TTF_SizeUTF8(tmp_font, l.c_str(), nullptr, &h0); // Get the temporary height
			h += h0 + lineskip;
		}

		TTF_CloseFont(tmp_font); // Close the temp font

		return h;
	}
	/**
	* @note When the function is called without a font size, let it be the Font's loaded size
	* @param text the string to get the height of
	*
	* @returns the height of the given text
	* @see get_string_width(const std::string&, int) const for details
	*/
	int Font::get_string_height(const std::string& text) const {
		return get_string_height(text, font_size);
	}
	/**
	* @note When the function is called without any arguments, let it the string be generic English and the size be the loaded size.
	*
	* @returns the height of a generic English string
	* @see get_string_width(const std::string&, int) const for details
	*/
	int Font::get_string_height() const {
		return get_string_height("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890`~!@#$%^&*(),./;'[]\\-='<>?:\"{}|_+", font_size);
	}
}

#endif // BEE_FONT
