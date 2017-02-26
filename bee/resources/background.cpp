/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_BACKGROUND
#define _BEE_BACKGROUND 1

#include "background.hpp" // Include the class resource header

/*
* BEE::BackgroundData::BackgroundData() - Construct the data struct and initiliaze all provided values
* ! See a description for these arguments in the struct definition in bee/resources/background.hpp
*/
BEE::BackgroundData::BackgroundData(BEE::Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
	init(new_background, new_is_visible, new_is_foreground, new_x, new_y, new_is_horizontal_tile, new_is_vertical_tile, new_horizontal_speed, new_vertical_speed, new_is_stretched);
}
/*
* BEE::BackgroundData::init() - Initliaze all members of the data struct
* ! See a description for these arguments in the struct definition in bee/resources/background.hpp
*/
int BEE::BackgroundData::init(BEE::Background* new_background, bool new_is_visible, bool new_is_foreground, int new_x, int new_y, bool new_is_horizontal_tile, bool new_is_vertical_tile, int new_horizontal_speed, int new_vertical_speed, bool new_is_stretched) {
	background = new_background;
	is_visible = new_is_visible;
	is_foreground = new_is_foreground;
	x = new_x;
	y = new_y;
	is_horizontal_tile = new_is_horizontal_tile;
	is_vertical_tile = new_is_vertical_tile;
	horizontal_speed = new_horizontal_speed;
	vertical_speed = new_vertical_speed;
	is_stretched = new_is_stretched;
	return 0;
}

/*
* BEE::Background::Background() - Construct the background, set its engine pointer, and reset all variables
* ! This constructor should only be used for temporary backgrounds (e.g. framebuffers), the other constructor should be used for all other cases
*/
BEE::Background::Background() {
	if (BEE::resource_list->backgrounds.game != nullptr) {
		game = BEE::resource_list->backgrounds.game; // Set the engine pointer
	}

	reset(); // Reset all resource variables
}
/*
* BEE::Background::Background() - Construct the background, reset all variables, add it to the background resource list, and set the new name and path
* @new_name: the name of the background to use
* @new_path: the path of the background's image
*/
BEE::Background::Background(std::string new_name, std::string new_path) {
	reset(); // Reset all resource variables

	add_to_resources(); // Add the background to the appropriate resource list
	if (id < 0) { // If the background could not be addedto the resource list, output a warning
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add background resource: \"" + new_name + "\" from " + new_path);
		throw(-1); // Throw an exception
	}

	set_name(new_name); // Set the background name
	set_path(new_path); // Set the background image path
}
/*
* BEE::Background::~Background() - Free the background data and remove it from the resource list
*/
BEE::Background::~Background() {
	this->free(); // Free all background data
	BEE::resource_list->backgrounds.remove_resource(id); // Remove the background from the resource list
}
/*
* BEE::Background::add_to_resources() - Add the background to the appropriate resource list
*/
int BEE::Background::add_to_resources() {
	if (id < 0) { // If the resource needs to be added to the resource list
		id = BEE::resource_list->backgrounds.add_resource(this); // Add the resource and get the new id
	}

	// Get the list's engine pointer if it's not nullptr
	if (BEE::resource_list->backgrounds.game != nullptr) {
		game = BEE::resource_list->backgrounds.game;
	}

	return 0; // Return 0 on success
}
/*
* BEE::Background::reset() - Reset all resource variables for initialization
*/
int BEE::Background::reset() {
	this->free(); // Free all memory used by the resource

	// Reset all properties
	name = "";
	path = "";
	width = 0;
	height = 0;
	animation_time = 0;

	// Reset texture data
	texture = nullptr;
	is_loaded = false;

	return 0; // Return 0 on success
}
/*
* BEE::Background::print() - Print all relevant information about the resource
*/
int BEE::Background::print() {
	std::stringstream s; // Declare the output stream
	s << // Append all info to the output
	"Background { "
	"\n	id              " << id <<
	"\n	name            " << name <<
	"\n	path            " << path <<
	"\n	width           " << width <<
	"\n	height          " << height <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str()); // Send the info to the messaging system for output

	return 0; // Return 0 on success
}
/*
* BEE::Background::get_*() - Return the requested resource information
*/
int BEE::Background::get_id() {
	return id;
}
std::string BEE::Background::get_name() {
	return name;
}
std::string BEE::Background::get_path() {
	return path;
}
int BEE::Background::get_width() {
	return width;
}
int BEE::Background::get_height() {
	return height;
}
bool BEE::Background::get_is_loaded() {
	return is_loaded;
}
SDL_Texture* BEE::Background::get_texture() {
	return texture;
}

/*
* BEE::Background::set_name() - Set the resource name
* @new_name: the new name to use for the resource
*/
int BEE::Background::set_name(std::string new_name) {
	name = new_name; // Set the name
	return 0; // Return 0 on success
}
/*
* BEE::Background::set_path() - Set the resource path
* @new_path: the new path to use for the resource
*/
int BEE::Background::set_path(std::string new_path) {
	path = "resources/backgrounds/"+new_path; // Append the path to the background directory
	return 0; // Return 0 on success
}
/*
* BEE::Background::set_time_update() - Update the starting time for a movement animation
*/
int BEE::Background::set_time_update() {
	animation_time = game->get_ticks(); // Update the animation starting time
	return 0; // Return 0 on success
}

/*
* BEE::Background::load_from_surface() - Load a texture from the given surface
* @tmp_surface: the temporary surface to load from
*/
int BEE::Background::load_from_surface(SDL_Surface* tmp_surface) {
	if (!is_loaded) { // Only attempt to load the background if it has not already been loaded
		// Set the background dimensions
		width = tmp_surface->w;
		height = tmp_surface->h;

		if (game->options->renderer_type != BEE_RENDERER_SDL) {
			// Generate the vertex array object for the background
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// Generate the four corner texcoords for the background
			GLfloat texcoords[] = {
				0.0, 0.0,
				1.0, 0.0,
				1.0, 1.0,
				0.0, 1.0,
			};
			glGenBuffers(1, &vbo_texcoords);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

			// Generate the four corner vertices of the rectangular background
			GLfloat vertices[] = {
				0.0,            0.0,
				(GLfloat)width, 0.0,
				(GLfloat)width, (GLfloat)height,
				0.0,            (GLfloat)height,
			};
			glGenBuffers(1, &vbo_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Generate the indices of the two triangle which form the rectangular background
			GLushort elements[] = {
				0, 1, 2,
				2, 3, 0,
			};
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

			// Bind the vertices to the VAO's vertex buffer
			glEnableVertexAttribArray(game->vertex_location);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glVertexAttribPointer(
				game->vertex_location,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
			);

			// Generate the texture from the surface pixels
			glGenTextures(1, &gl_texture);
			glBindTexture(GL_TEXTURE_2D, gl_texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RGBA,
				width,
				height,
				0,
				GL_RGBA,
				GL_UNSIGNED_BYTE,
				tmp_surface->pixels
			);

			glBindVertexArray(0); // Unbind the VAO when done loading

			// Set loaded booleans
			is_loaded = true;
			has_draw_failed = false;
		} else {
			// Generate an SDL texture from the surface pixels
			texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
			if (texture == nullptr) { // If the texture could not be generated, output a warning
				game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to create texture from surface for \"" + name + "\": " + get_sdl_error());
				return 2; // Return 2 on failure to load
			}

			// Set loaded booleans
			is_loaded = true;
			has_draw_failed = false;
		}
	} else { // If the sprite has already been loaded, output a warning
		game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to load background \"" + name + "\" from surface because it has already been loaded");
		return 1; // Return 1 on failure
	}

	return 0; // Return 0 on success
}
/*
* BEE::Background::load() - Load the background from its given filename
*/
int BEE::Background::load() {
	if (!is_loaded) { // Only attempt to load the background if it has not already been loaded
		// Load the image into a temporary surface
		SDL_Surface* tmp_surface;
		tmp_surface = IMG_Load(path.c_str());
		if (tmp_surface == nullptr) { // If the surface could not be loaded, output a warning
			game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to load background " + name + ": " + IMG_GetError());
			return 1; // Return 1 on load failure
		}

		load_from_surface(tmp_surface); // Load the surface into a texture
		SDL_FreeSurface(tmp_surface); // Free the temporary surface
	} else { // If the background has already been loaded, output a warning
		game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to load background \"" + name + "\" because it has already been loaded");
		return 1; // Return 1 on failure
	}

	return 0; // Return 0 on success
}
/*
* BEE::Background::free() - Free the background texture and delete all of its buffers
*/
int BEE::Background::free() {
	if (is_loaded) { // Do not attempt to free the texture if it has not been loaded
		if (game->options->renderer_type != BEE_RENDERER_SDL) {
			// Delete the vertex and index buffer
			glDeleteBuffers(1, &vbo_vertices);
			glDeleteBuffers(1, &ibo);

			// Delete the texture, the texture coordinates, and the optional framebuffer
			glDeleteTextures(1, &gl_texture);
			glDeleteBuffers(1, &vbo_texcoords);
			glDeleteFramebuffers(1, &framebuffer);

			// Finally delete the VAO
			glDeleteVertexArrays(1, &vao);
		} else {
			// Delete the SDL texture
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
		is_loaded = false; // Set the loaded boolean
	}

	return 0; // Return 0 on success
}
/*
* BEE::Background::draw_internal() - Draw the given crop of the background onto the given area of the window
* @src: the rectangle of the background to draw
* @dest: the rectangle of the window to draw to
*/
int BEE::Background::draw_internal(const SDL_Rect* src, const SDL_Rect* dest) const {
	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		glBindVertexArray(vao); // Bind the VAO for the background

		// Generate the partial transformation matrix (translation and scaling) for the subimage
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3((float)dest->w/width, (float)dest->h/height, 1.0f)); // Scale the texture in the x- and y-planes
		model = glm::translate(model, glm::vec3((float)dest->x, (float)dest->y, 0.0f)); // Translate the texture the desired amount in the x- and y-planes
		glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(model)); // Send the transformation matrix to the shader

		// Bind the background texture
		glUniform1i(game->texture_location, 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

		// Bind the texture coordinates
		glEnableVertexAttribArray(game->fragment_location);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords);
		glVertexAttribPointer(
			game->fragment_location,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);

		// Draw the triangles which form the rectangular background
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		// Reset the shader state
		glDisableVertexAttribArray(game->fragment_location); // Unbind the texture coordinates

		glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the partial transformation matrix
		glUniformMatrix4fv(game->rotation_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the rotation matrix
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the sprite texture
		glUniform1i(game->flip_location, 0); // Reset the flip type

		glBindVertexArray(0); // Unbind the VAO
	} else {
		SDL_RenderCopy(game->renderer, texture, src, dest); // Render the background
	}

	return 0; // Return 0 on success
}
/*
* BEE::Background::tile_horizontal() - Draw the background multiple times across the window's width
* @r: the starting rectangle of the window
*/
int BEE::Background::tile_horizontal(const SDL_Rect* r) const {
	if (r->w <= 0) { // Do not attempt to draw if the width of the rectangle is non-positive
		return -1; // Return -1 on failure
	}

	// Declare the initial drawing rectangles
	SDL_Rect src = {0, 0, r->w, r->h};
	SDL_Rect dest = {r->x, r->y, r->w, r->h};

	int i=0; // Store the amount of times that the background is drawn
	while (dest.x < game->get_room_width()) { // Continue drawing to the right until the rectangle is past the right side of the window
		draw_internal(&src, &dest); // Draw the desired rectangle
		i++; // Increment the draw amount

		dest.x += dest.w; // Move the rectangle on right to the next tile
	}

	dest.x = r->x - dest.w; // Reset the rectangle start
	while (dest.x + dest.w > 0) { // Continue drawing to the left until the rectangle is past the left side of the window
		draw_internal(&src, &dest); // Draw the desired rectangle
		i++; // Increment the draw amount

		dest.x -= dest.w; // Move the rectangle on left to the next tile
	}

	return i; // Return the amount of drawn tiles on success
}
/*
* BEE::Background::tile_vertical() - Draw the background multiple times across the window's height
* @r: the starting rectangle of the window
*/
int BEE::Background::tile_vertical(const SDL_Rect* r) const {
	if (r->h <= 0) { // Do not attempt to draw if the height of the rectangle is non-positive
		return -1; // Return -1 on failure
	}

	// Declare the initial drawing rectangles
	SDL_Rect src = {0, 0, r->w, r->h};
	SDL_Rect dest = {r->x, r->y, r->w, r->h};

	int i=0; // Store the amount of times that the background is drawn
	while (dest.y < game->get_room_height()) { // Continue drawing to the bottom until the rectnagle is past the bottom of the window
		draw_internal(&src, &dest); // Draw the deired rectangle
		i++; // Increment the draw amount

		dest.y += r->h; // Move the rectangle on down to the next tile
	}

	dest.y = r->y - dest.h; // Reset the rectangle start
	while (dest.y + dest.h > 0) { // Continue drawing to the top until the rectangle is past the top of the window
		draw_internal(&src, &dest); // Draw the desired rectangle
		i++; // Increment the draw amount

		dest.y -= r->h; // Move the rectangle on up to the next tile
	}

	return i; // Return the amount of drawn tiles on success
}
/*
* BEE::Background::draw() - Draw the background at the given coordinates with the given properties
* @x: the x-coordinate to draw the background at
* @y: the y-coordinate to draw the background at
* @b: the background data to use when drawing
*/
int BEE::Background::draw(int x, int y, BackgroundData* b) {
	if (!is_loaded) { // Do not attempt to draw the background if it has not been loaded
		if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
		game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to draw background \"" + name + "\" because it is not loaded");
		has_draw_failed = true; // Set the draw failure boolean
		}
		return 1; // Return 1 on failure
	}

	if (b->is_stretched) { // If the background should be stretched, then draw it without animation
		SDL_Rect rect = {0, 0, game->get_room_width(), game->get_room_height()}; // Declare the drawing rectangle with the full width and height of the window
		draw_internal(nullptr, &rect); // Draw the background
	} else { // Otherwise, draw the background with optional animation and tiling
		// Calculate the delta x and y for the current animation time
		int dx = b->horizontal_speed*(game->get_ticks()-animation_time)/game->fps_goal;
		int dy = b->vertical_speed*(game->get_ticks()-animation_time)/game->fps_goal;

		SDL_Rect rect = {x+dx, y+dy, (int)width, (int)height}; // Declare the drawing rectangle with the moved coordinates

		if ((rect.w > 0)&&(rect.h > 0)) { // Only attempt to draw the background if it's dimensions are positive
			if (b->is_horizontal_tile && b->is_vertical_tile) { // If the background should be tiled in both directions, tile it
				while (rect.y-rect.h < game->get_room_height()) { // Tile as many horizontal lines as necessary to fill the window to the bottom
					tile_horizontal(&rect); // Tile the background across the row
					rect.y += rect.h; // Move to the below row
				}
				rect.y = y + dy - rect.h; // Reset the row to
				while (rect.y+rect.h > 0) { // Tile as many horizontal lines as necessary to fill the window to the top
					tile_horizontal(&rect); // Tile the background across the row
					rect.y -= rect.h; // Move to the above row
				}
			} else if (b->is_horizontal_tile) { // If the background should only be tiled horizontally, tile it
				tile_horizontal(&rect);
			} else if (b->is_vertical_tile) { // If the background should only be tiled vertically, tile it
				tile_vertical(&rect);
			} else { // If the background should not be tiled, draw it normally
				draw_internal(nullptr, &rect);
			}
		}
	}

	return 0; // Return 0 on success
}
/*
* BEE::Background::set_as_target() - Set up the background for use as a render target
* @w: the width of the render texture
* @h: the height of the render texture
*/
int BEE::Background::set_as_target(int w, int h) {
	if (is_loaded) { // If the sprite is already loaded, free the old data
		this->free();
	}

	// Set the background dimensions
	width = w;
	height = h;

	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		// Generate the vertex array object for the background
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate the four corner vertices of the rectangular background
		GLfloat vertices[] = {
			0.0,            0.0,
			(GLfloat)width, 0.0,
			(GLfloat)width, (GLfloat)height,
			0.0,            (GLfloat)height,
		};
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Generate the indices of the two triangles which form the rectangular background
		GLushort elements[] = {
			0, 1, 2,
			2, 3, 0,
		};
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		// Bind the vertices to the VAO's vertex buffer
		glEnableVertexAttribArray(game->vertex_location);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(
			game->vertex_location,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);

		// Generate the framebuffer that will be used to render to this background
		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// Generate an empty texture
		glGenTextures(1, &gl_texture);
		glBindTexture(GL_TEXTURE_2D, gl_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			width,
			height,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			nullptr
		);

		// Bind the framebuffer to the empty texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_texture, 0);
		GLenum buffer[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(framebuffer, buffer); // Enable drawing to the framebuffer

		// Check whether the framebuffer has been successfully initialized
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { // If not, reset the state
			game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to create a new framebuffer.");
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer to switch back to the default
			this->free(); // Free the old data
			return 0; // Return 0 on failure
		}

		glBindVertexArray(0); // Unbind VAO when done loading

		return (int)framebuffer; // Return the framebuffer index on success
	} else {
		texture = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h); // Create an empty texture
		if (texture == nullptr) { // If the texture could not be created, output a warning
			game->messenger_send({"engine", "background"}, BEE_MESSAGE_WARNING, "Failed to create a blank texture: " + get_sdl_error());
			return 0; // Return 0 on failure
		}

		SDL_SetRenderTarget(game->renderer, texture); // Set the SDL render target
	}

	// Set loaded booleans
	is_loaded = true;
	has_draw_failed = false;

	return 1; // Return a positive integer on success
}
/*
* BEE::Background::set_as_target() - Set up the background for use as a render target
* ! When the function is called without specifying width and height, simply call it with the window dimensions
*/
int BEE::Background::set_as_target() {
	return set_as_target(game->get_width(), game->get_height()); // Return the result of attempting to set the render target
}

#endif // _BEE_BACKGROUND
