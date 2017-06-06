/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_FONT
#define _BEE_FONT 1

#include "font.hpp" // Include the class resource header

/*
* BEE::TextData::TextData() - Construct the data struct and initialize the default values
*/
BEE::TextData::TextData() :
	sprites(),
	text()
{}
/*
* BEE::TextData::TextData() - Construct the data struct and initialize the provided values
* @new_sprite: the sprite for the first line of text
* @new_text: the text that has been rendered in the sprite
*/
BEE::TextData::TextData(BEE::Sprite* new_sprite, const std::string& new_text) :
	sprites(),
	text(new_text)
{
	sprites.emplace(0, new_sprite); // Set the provided sprite as the data for the first line
}
/*
* BEE::TextData::~TextData() - Free the memory for each line's sprite
*/
BEE::TextData::~TextData() {
	for (auto& s : sprites) { // Iterate over the sprites and delete each one
		delete s.second;
	}
	sprites.clear(); // Clear the list
	text.clear();
}
/*
* BEE::TextData::pop_front() -  Pop the front sprite so it won't be deleted twice
*/
BEE::Sprite* BEE::TextData::pop_front() {
	Sprite* s = sprites.begin()->second; // Store a copy of the sprite pointer
	sprites.erase(sprites.begin()); // Erase the sprite from the list
	return s; // Return the sprite
}

/*
* BEE::Font::Font() - Construct the font and set its engine pointer
* ! This constructor should only be directly used for temporary fonts, the other constructor should be used for all other cases
*/
BEE::Font::Font() :
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
{
	// Get the list's engine pointer if it's not nullptr
	if (BEE::resource_list->fonts.game != nullptr) {
		game = BEE::resource_list->fonts.game; // Set the engine pointer
	}
}
/*
* BEE::Font::Font() - Construct the font, reset all variables, add it to the font resource list, and set the new name and path
* @new_name: the name of the font to use
* @new_path: the path of the font file, must be either a TTF or bitmap font
* @new_font_size: the size of the font to use
* @new_is_sprite: whether the font is a sprite (bitmap) font
*/
BEE::Font::Font(const std::string& new_name, const std::string& new_path, int new_font_size, bool new_is_sprite) :
	Font() // Default initialize all variables
{
	add_to_resources(); // Add the font to the appropriate resource list
	if (id < 0) { // If the font could not be added to the resource list, output a warning
		game->messenger_send({"engine", "resource"}, bee::E_MESSAGE::WARNING, "Failed to add font resource: \"" + new_name + "\" from " + new_path);
		throw(-1); // Throw an exception
	}

	set_name(new_name); // Set the font name
	set_path(new_path); // Set the font path
	set_font_size(new_font_size); // Set the font size
	is_sprite = new_is_sprite; // Directly set whether the font is a sprite font
}
/*
* BEE::Font::~Font() - Free the font data and remove it from the resource list
*/
BEE::Font::~Font() {
	this->free(); // Free all font data
	BEE::resource_list->fonts.remove_resource(id); // Remove the font from the resource list
}
/*
* BEE::Font::add_to_resources() - Add the font to the appropriate resource list
*/
int BEE::Font::add_to_resources() {
	if (id < 0) { // If the resource needs to be added to the resource list
		id = BEE::resource_list->fonts.add_resource(this); // Add the resource and get the new id
	}

	return 0; // Return 0 on success
}
/*
* BEE::Font::reset() - Reset all resource variables for reinitialization
*/
int BEE::Font::reset() {
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
* BEE::Font::print() - Print all relevant information about the resource
*/
int BEE::Font::print() const {
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
	game->messenger_send({"engine", "resource"}, bee::E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

	return 0; // Return 0 on success
}

/*
* BEE::Font::get_*() - Return the requested resource information
*/
int BEE::Font::get_id() const {
	return id;
}
std::string BEE::Font::get_name() const {
	return name;
}
std::string BEE::Font::get_path() const {
	return path;
}
int BEE::Font::get_font_size() const {
	return font_size;
}
int BEE::Font::get_style() const {
	return style;
}
int BEE::Font::get_lineskip() const {
	return lineskip;
}
int BEE::Font::get_lineskip_default() {
	if (!is_loaded) { // If the font is not loaded, output a warning
		game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to get the default lineskip for \"" + name + "\" becuase it is not loaded");
		return -1; // Return -1 on failure
	}

	return TTF_FontLineSkip(font); // Return the lineskip on success
}
std::string BEE::Font::get_fontname() {
	std::string fontname = ""; // Create the fontname in the following format "family style", i.e. "Liberation Mono Regular"
	fontname.append(TTF_FontFaceFamilyName(font));
	fontname.append(" ");
	fontname.append(TTF_FontFaceStyleName(font));
	return fontname;
}

/*
* BEE::Font::set_name() - Set the resource name
* @new_name: the new name to use for the resource
*/
int BEE::Font::set_name(const std::string& new_name) {
	name = new_name; // Set the name
	return 0; // Return 0 on success
}
/*
* BEE::Font::set_path() - Set the resource path
* @new_path: the new path to use for the resource
*/
int BEE::Font::set_path(const std::string& new_path) {
	path = "resources/fonts/"+new_path; // Set the path
	return 0; // Return 0 on success
}
/*
* BEE::Font::set_font_size() - Set the font size to use when loading the font
* @new_font_size: the size to use
*/
int BEE::Font::set_font_size(int new_font_size) {
	font_size = new_font_size; // Set the font size
	return 0; // Return 0 on success
}
/*
* BEE::Font::set_style() - Set the font style to use with the font
* @new_style: the desired style, a bitmask of TTF_STYLE_BOLD, _ITALIC, _UNDERLINE, _STRIKETHROUGH, and _NORMAL
*/
int BEE::Font::set_style(int new_style) {
	if (!is_loaded) { // Do not attempt to set the style if the font has not been loaded
		game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to set the font style for \"" + name + "\" because it is not loaded");
		return 1; // Return 1 on error
	}

	style = new_style; // Store the style
	TTF_SetFontStyle(font, style); // Set the style of the loaded font
	return 0; // Return 0 on success
}
/*
* BEE::Font::set_lineskip() - Set the lineskip to use with the font
* @new_lineskip: the lineskip to use
*/
int BEE::Font::set_lineskip(int new_lineskip) {
	lineskip = new_lineskip; // Set the lineskip
	return 0; // Return 0 on success
}

/*
* BEE::Font::load() - Load the font from its given filename
*/
int BEE::Font::load() {
	if (is_loaded) { // If the font has already been loaded, output a warning
		game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to load font \"" + name + "\" from because it has already been loaded");
		return 1; // Return 1 when already loaded
	}

	if (is_sprite) { // If the font is a sprite font, load it appropriately
		sprite_font = new Sprite("spr_"+get_name(), path); // Create and load a sprite for the font
		if (!sprite_font->load()) { // If the sprite fails to load, output a warning
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to load the font \"" + path + "\": " + get_sdl_error());
			has_draw_failed = true;
			return 2; // Return 2 on loading failure
		}

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;
	} else { // Otherwise load the sprite's TTF file
		if (game->options->renderer_type != bee::E_RENDERER::SDL) { // If the engine is rendering in OpenGL mode, output a warning about fast font drawing
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Please note that TTF fast font rendering is currently broken in OpenGL mode\nThe current behavior is to draw slowly and discard the texture data");
		}

		font = TTF_OpenFont(path.c_str(), font_size); // Open the TTF file with the desired font size
		if (font == nullptr) { // If the font failed to load, output a warning
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to load font \"" + path + "\": " + TTF_GetError());
			has_draw_failed = true;
			return 2; // Return 2 on loading failure
		}

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;
	}

	return 0; // Return 0 on success
}
/*
* BEE::Font::free() - Free the font textures and other data
*/
int BEE::Font::free() {
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
* BEE::Font::draw_internal() - Draw the given text with the given attributes
* @x: the x-coordinate to draw the text at
* @y: the y-coordinate to draw the text at
* @text: the string to draw
* @color: the color to draw the text in
*/
BEE::TextData* BEE::Font::draw_internal(int x, int y, const std::string& text, RGBA color) {
	if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
		if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
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
		game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to draw with font \"" + name + "\": " + TTF_GetError());
		return nullptr; // Return nullptr when rendering failed
	}

	// Create a temporary sprite to draw the rendered text to the screen
	Sprite* tmp_sprite = new Sprite();
	tmp_sprite->load_from_surface(tmp_surface); // Load the rendered text into the sprite
	SDL_FreeSurface(tmp_surface); // Free the temporary surface

	tmp_sprite->set_is_lightable(false); // Remove lighting from the text
	tmp_sprite->draw(x, y, 0, false); // Draw the text

	TextData* textdata = new TextData(tmp_sprite, t); // Store the temporary sprite in a TextData struct

	return textdata; // Return the textdata on success
}
/*
* BEE::Font::draw() - Draw the given text with the given attributes
* @x: the x-coordinate to draw the text at
* @y: the y-coordinate to draw the text at
* @text: the string to draw
* @color: the color to draw the text in
*/
BEE::TextData* BEE::Font::draw(int x, int y, const std::string& text, RGBA color) {
	if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
		if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
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
				textdata->sprites.emplace(textdata->sprites.size(), r->pop_front());
				delete r; // Free the temporary data
			}
		}
	}
	return textdata; // Return the textdata on success
}
/*
* BEE::Font::draw() - Draw the given text at the given coordinates
* ! When the function is called without a color, simply draw it in black
* @x: the x-coordinate to draw the text at
* @y: the y-coordinate to draw the text at
* @text: the string to draw
*/
BEE::TextData* BEE::Font::draw(int x, int y, const std::string& text) {
	return draw(x, y, text, {0, 0, 0, 255}); // Return the result of drawing the text in black
}
/*
* BEE::Font::draw() - Draw the given text with the given attributes
* ! Note that the user is only responsible for freeing the returned TextData after the last time that they draw their text
* ! This allows the following syntax:
	td = font->draw(td, x, y, text, color);
* @textdata: the textdata to draw if the given text matches the prerendered text
* @x: the x-coordinate to draw the text at
* @y: the y-coordinate to draw the text at
* @text: the string to draw
* @color: the color to draw the text in
*/
BEE::TextData* BEE::Font::draw(TextData* textdata, int x, int y, const std::string& text, RGBA color) {
	if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
		if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
			has_draw_failed = true; // Set the draw failure boolean
		}
		return nullptr; // Return nullptr when not loaded
	}

	if ((textdata != nullptr)&&(textdata->text == text)) { // If the prerendered text matches the given text, draw it as-is
		std::vector<std::string> lines = splitv(text, '\n', false); // Separate the text by newline
		size_t i = 0;
		for (auto& l : lines) { // Iterate over the lines and draw each one
			if (!l.empty()) {
				textdata->sprites[i]->draw(x, y, 0, false);
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
* BEE::Font::draw() - Draw the given text at the given coordinates
* ! When the function is called without a color, simply draw it in black
* @textdata: the textdata to draw if the given text matches the prerendered text
* @x: the x-coordinate to draw the text at
* @y: the y-coordinate to draw the text at
* @text: the string to draw
*/
BEE::TextData* BEE::Font::draw(TextData* textdata, int x, int y, const std::string& text) {
	return draw(textdata, x, y, text, {0, 0, 0, 255}); // Return the result of drawing the text in black
}

/*
* BEE::Font::draw_fast_internal() - Draw the given text with the given attributes without storing the rendered text
* @x: the x-coordinate to draw the text at
* @y: the y-coordinate to draw the text at
* @text: the string to draw
* @color: the color to draw the text in
*/
int BEE::Font::draw_fast_internal(int x, int y, const std::string& text, RGBA color) {
	if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
		if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
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
			sprite_font->draw_subimage(x+(i++), y, (int)c, w, h, 0.0, {color.r, color.g, color.b, color.a}, SDL_FLIP_NONE);
		}
	} else { // Otherwise, draw the font normally
		// Render the text to a temporary surface
		SDL_Surface* tmp_surface;
		if (game->options->renderer_type != bee::E_RENDERER::SDL) { // Since fast font rendering is currently broken in OpenGL, fallback to the slower rendering
			tmp_surface = TTF_RenderUTF8_Blended(font, t.c_str(), {color.r, color.g, color.b, color.a}); // Use the slow but pretty TTF rendering mode
		} else {
			tmp_surface = TTF_RenderUTF8_Solid(font, t.c_str(), {color.r, color.g, color.b, color.a}); // Use the fast but ugly TTF rendering mode
		}
		if (tmp_surface == nullptr) { // If the text failed to render, output a warning
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to draw with font \"" + name + "\": " + TTF_GetError());
			return 2; // Return 2 on rendering error
		}

		// Create a temporary sprite to draw the rendered text to the screen
		Sprite* tmp_sprite = new Sprite();
		tmp_sprite->load_from_surface(tmp_surface); // Load the rendered text into the sprite
		SDL_FreeSurface(tmp_surface); // Free the temporary surface

		tmp_sprite->set_is_lightable(false); // Remove lighting from the text
		tmp_sprite->draw(x, y, 0, false); // Draw the text

		delete tmp_sprite; // Free the temporary sprite
	}

	return 0; // Return 0 on success
}
/*
* BEE::Font::draw_fast() - Draw the given text with the given attributes without storing the rendered text
* @x: the x-coordinate to draw the text at
* @y: the y-coordinate to draw the text at
* @text: the string to draw
* @color: the color to draw the text in
*/
int BEE::Font::draw_fast(int x, int y, const std::string& text, RGBA color) {
	if (!is_loaded) { // Do not attempt to draw the text if the font has not been loaded
		if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
			game->messenger_send({"engine", "font"}, bee::E_MESSAGE::WARNING, "Failed to draw text with \"" + name + "\" because it is not loaded");
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
* BEE::Font::draw_fast() - Draw the given text with the given attributes without storing the rendered text
* ! When the function is called without a color, simply draw it in black
* @x: the x-coordinate to draw the text at
* @y: the y-coordinate to draw the text at
* @text: the string to draw
*/
int BEE::Font::draw_fast(int x, int y, const std::string& text) {
	return draw_fast(x, y, text, {0, 0, 0, 255}); // Return the result of drawing the text in black
}

/*
* BEE::Font::get_string_width() - Return the width of the given text in the given font size
* @text: the string to get the width of
* @size: the font size to use when getting the string width
*/
int BEE::Font::get_string_width(const std::string& text, int size) const {
	if ((!is_loaded)||(is_sprite)) { // Do not attempt to get the string width if the font has not been loaded
		return -1; // Return -1 when not loaded
	}

	int w = 0; // Declare a temporary variable for the width
	if (size == font_size) { // If the desired size is the same as the currently loaded size, fetch the width appropriately
		TTF_SizeUTF8(font, text.c_str(), &w, nullptr);
	} else { // Otherwise, load a temporary font
		TTF_Font* tmp_font = TTF_OpenFont(path.c_str(), size); // Open the same TTF file with the desired font size
		if (tmp_font == nullptr) { // If the font failed to load, output a warning
			return -2; // Return -2 when font loading failed
		}

		TTF_SizeUTF8(font, text.c_str(), &w, nullptr); // Get the temporary width

		TTF_CloseFont(tmp_font); // Close the font after getting the width
	}

	return w; // Return the width on success
}
/*
* BEE::Font::get_string_width() - Return the width of the given text
* ! When the function is called without a font size, simply call it with the font's loaded size
* @text: the string to get the width of
*/
int BEE::Font::get_string_width(const std::string& text) const {
	return get_string_width(text, font_size); // Return the width in the current font size
}
/*
* BEE::Font::get_string_height() - Return the height of the given text in the given font size
* @text: the string to get the height of
* @size: the font size to use when getting the string height
*/
int BEE::Font::get_string_height(const std::string& text, int size) const {
	if ((!is_loaded)||(is_sprite)) { // Do not attempt to get the string height if the font has not been loaded
		return -1; // Return -1 when not loaded
	}

	int h = 0; // Declare a temporary variable for the height
	if (size == font_size) {
		TTF_SizeUTF8(font, text.c_str(), nullptr, &h);
	} else { // Otherwise, load a temporary font
		TTF_Font* tmp_font = TTF_OpenFont(path.c_str(), size); // Open the same TTF file with the desired font size
		if (tmp_font == nullptr) { // If the font failed to load, output a warning
			return -2; // Return -2 when font loading failed
		}

		TTF_SizeUTF8(font, text.c_str(), nullptr, &h); // Get the temporary height

		TTF_CloseFont(tmp_font); // Close the font after getting the height
	}

	return h; // Return the height on success
}
/*
* BEE::Font::get_string_height() - Return the height of the given text
* ! When the function is called without a font size, simply call it with the font's loaded size
* @text: the string to get the height of
*/
int BEE::Font::get_string_height(const std::string& text) const {
	return get_string_height(text, font_size); // Return the height in the current font size
}
/*
* BEE::Font::get_string_height() - Return the height of a generic English string
* ! When the function is called without any arguments, simply call it with the a generic English string and the font's loaded size
*/
int BEE::Font::get_string_height() const {
	return get_string_height("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890`~!@#$%^&*(),./;'[]\\-='<>?:\"{}|_+", font_size); // Return the height of most general characters in the current font size
}

#endif // _BEE_FONT
