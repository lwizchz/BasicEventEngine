/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_SPRITE
#define BEE_SPRITE 1

#include "../defines.hpp"

#include <sstream> // Include the required library headers

#include <SDL2/SDL_image.h> // Include the required SDL headers

#include <glm/gtc/matrix_transform.hpp> // Include the required OpenGL headers
#include <glm/gtc/type_ptr.hpp>

#include "sprite.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/real.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/rooms.hpp"
#include "../core/window.hpp"

#include "../render/drawing.hpp"
#include "../render/renderer.hpp"

#include "light.hpp"
#include "room.hpp"

namespace bee {
	/*
	* SpriteDrawData::SpriteDrawData() - Construct the data struct and initialize all values
	* ! See bee/resources/sprite.hpp for a description of these member variables
	*/
	SpriteDrawData::SpriteDrawData() :
		x(0),
		y(0),
		subimage_time(0),
		w(0),
		h(0),
		angle(0.0)
	{}
	/*
	* SpriteDrawData::SpriteDrawData() - Construct the data struct and initialize with all the given values
	* ! See bee/resources/sprite.hpp for a description of these member variables
	*/
	SpriteDrawData::SpriteDrawData(int new_x, int new_y, Uint32 new_subimage_time, int new_w, int new_h, double new_angle):
		x(new_x),
		y(new_y),
		subimage_time(new_subimage_time),
		w(new_w),
		h(new_h),
		angle(new_angle)
	{}

	std::map<int,Sprite*> Sprite::list;
	int Sprite::next_id = 0;

	/*
	* Sprite::Sprite() - Default construct the sprite
	* ! This constructor should only be directly used for temporary sprites (e.g. framebuffers), the other constructor should be used for all other cases
	*/
	Sprite::Sprite() :
		Resource(),

		id(-1),
		name(),
		path(),
		width(0),
		height(0),
		subimage_amount(1),
		subimage_width(0),
		crop({0,0,0,0}),
		speed(0.0),
		is_animated(false),
		origin_x(0),
		origin_y(0),
		rotate_x(0.5),
		rotate_y(0.5),

		texture(nullptr),
		is_loaded(false),
		has_draw_failed(false),
		subimages(),

		vao(-1),
		vbo_vertices(-1),
		ibo(-1),
		gl_texture(-1),
		vbo_texcoords(),
		is_lightable(true),

		framebuffer(-1)
	{}
	/*
	* Sprite::Sprite() - Construct the sprite, add it to the sprite resource list, and set the new name and path
	* @new_name: the name of the sprite to use
	* @new_path: the path of the sprite's image
	*/
	Sprite::Sprite(const std::string& new_name, const std::string& new_path) :
		Sprite() // Default initialize all variables
	{
		add_to_resources(); // Add the sprite to the appropriate resource list
		if (id < 0) { // If the sprite could not be added, output a warning
			messenger::send({"engine", "resource"}, E_MESSAGE::WARNING, "Failed to add sprite resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception
		}

		set_name(new_name);
		set_path(new_path);
	}
	/*
	* Sprite::~Sprite() - Free the sprite data and remove it from the resource list
	*/
	Sprite::~Sprite() {
		this->free(); // Free all sprite data
		list.erase(id); // Remove the sprite from the resource list
	}

	/*
	* Sprite::add_to_resources() - Add the sprite to the appropriate resource list
	*/
	int Sprite::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return 0; // Return 0 on success
	}
	/*
	* Sprite::get_amount() - Return the amount of sprite resources
	*/
	size_t Sprite::get_amount() {
		return list.size();
	}
	/*
	* Sprite::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Sprite* Sprite::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Sprite::reset() - Reset all resource variables for reinitialization
	*/
	int Sprite::reset() {
		this->free(); // Free all memory used by this resource

		// Reset all properties
		name = "";
		path = "";
		width = 0;
		height = 0;
		subimage_amount = 1;
		subimage_width = 0;
		crop = {0, 0, 0, 0};
		speed = 0.0;
		is_animated = false;
		origin_x = 0;
		origin_y = 0;
		rotate_x = 0.5;
		rotate_y = 0.5;

		// Reset texture data
		texture = nullptr;
		is_loaded = false;
		subimages.clear();
		has_draw_failed = false;
		is_lightable = true;

		return 0; // Return 0 on success
	}
	/*
	* Sprite::print() - Print all relevant information about the resource
	*/
	int Sprite::print() const {
		std::stringstream s; // Declare the output stream
		s << // Append all info to the output
		"Sprite { "
		"\n	id              " << id <<
		"\n	name            " << name <<
		"\n	path            " << path <<
		"\n	width           " << width <<
		"\n	height          " << height <<
		"\n	subimage_amount " << subimage_amount <<
		"\n	subimage_width  " << subimage_width <<
		"\n	crop            {" << crop.x << "x, " << crop.y << "y, " << crop.w << "w, " << crop.h << "h}" <<
		"\n	speed           " << speed <<
		"\n	origin_x        " << origin_x <<
		"\n	origin_y        " << origin_y <<
		"\n	rotate_x        " << rotate_x <<
		"\n	rotate_y        " << rotate_y <<
		"\n	texture         " << texture <<
		"\n	is_loaded       " << is_loaded <<
		"\n	has_draw_failed " << has_draw_failed <<
		"\n	is_lightable    " << is_lightable <<
		"\n}\n";
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Sprite::get_*() - Return the requested resource information
	*/
	int Sprite::get_id() const {
		return id;
	}
	std::string Sprite::get_name() const {
		return name;
	}
	std::string Sprite::get_path() const {
		return path;
	}
	int Sprite::get_width() const {
		return width;
	}
	int Sprite::get_height() const {
		return height;
	}
	int Sprite::get_subimage_amount() const {
		return subimage_amount;
	}
	int Sprite::get_subimage_width() const {
		return subimage_width;
	}
	double Sprite::get_speed() const {
		return speed;
	}
	bool Sprite::get_is_animated() const {
		return is_animated;
	}
	int Sprite::get_origin_x() const {
		return origin_x;
	}
	int Sprite::get_origin_y() const {
		return origin_y;
	}
	double Sprite::get_rotate_x() const {
		return rotate_x;
	}
	double Sprite::get_rotate_y() const {
		return rotate_y;
	}
	SDL_Texture* Sprite::get_texture() const {
		return texture;
	}
	bool Sprite::get_is_loaded() const {
		return is_loaded;
	}
	bool Sprite::get_is_lightable() const {
		return is_lightable;
	}

	/*
	* Sprite::set_*() - Set the requested resource data
	*/
	int Sprite::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Sprite::set_path(const std::string& new_path) {
		path = "resources/sprites/"+new_path; // Append the path to the sprite directory
		return 0;
	}
	int Sprite::set_speed(double new_speed) {
		speed = new_speed;
		return 0;
	}
	int Sprite::set_origin_xy(int new_origin_x, int new_origin_y) {
		origin_x = new_origin_x;
		origin_y = new_origin_y;
		return 0;
	}
	int Sprite::set_origin_x(int new_origin_x) {
		return set_origin_xy(new_origin_x, get_origin_y());
	}
	int Sprite::set_origin_y(int new_origin_y) {
		return set_origin_xy(get_origin_x(), new_origin_y);
	}
	int Sprite::set_origin_center() {
		return set_origin_xy(subimage_width/2, height/2);
	}
	/*
	* Sprite::set_rotate_xy() - Set both coordinates of the rotation origin
	* ! Note that all x- and y-coordinates of the rotation origin are given as a percentage of the width and height of the image
	* @new_rotate_x: the new x-coordinate to rotate the sprite around
	* @new_rotate_y: the new y-coordinate to rotate the sprite around
	*/
	int Sprite::set_rotate_xy(double new_rotate_x, double new_rotate_y) {
		rotate_x = new_rotate_x;
		rotate_y = new_rotate_y;
		return 0;
	}
	int Sprite::set_rotate_x(double new_rotate_x) {
		return set_rotate_xy(new_rotate_x, get_rotate_y());
	}
	int Sprite::set_rotate_y(double new_rotate_y) {
		return set_rotate_xy(get_rotate_x(), new_rotate_y);
	}
	int Sprite::set_rotate_center() {
		return set_rotate_xy(0.5, 0.5);
	}
	/*
	* Sprite::set_is_lightable() - Set whether the rendered fragments should be affected by lighting and shadows
	* ! Normally this should only be used for specific HUD elements or text
	* ! See set_is_lightable() in bee/render/render.cpp for more general usage
	*/
	int Sprite::set_is_lightable(bool new_is_lightable) {
		is_lightable = new_is_lightable;
		return 0;
	}
	/*
	* Sprite::set_subimage_amount() - Set subimage coordinates and generate OpenGL buffers if necessary
	* @new_subimage_amount: the amount of subimages to use
	* @new_subimage_width: the width of each subimage
	*/
	int Sprite::set_subimage_amount(int new_subimage_amount, int new_subimage_width) {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		// Reset the subimage properties
		subimages.clear();
		subimage_amount = new_subimage_amount;
		subimage_width = new_subimage_width;

		// Generate the subimage coordinates (mainly used for SDL rendering)
		for (size_t i=0; i<subimage_amount; i++) {
			subimages.push_back({static_cast<int>(i*subimage_width), 0, static_cast<int>(subimage_width), static_cast<int>(height)});
		}

		if (get_options().renderer_type == E_RENDERER::SDL) { // If SDL rendering is being used, exit early
			return 0; // Return 0 on success
		}

		// Destroy all old texcoords
		if (!vbo_texcoords.empty()) {
			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();
		}

		// Convert the subimage width to a percentage of the full texture width
		GLfloat w = GLfloat(subimage_width);
		if (width > 0) {
			w /= width;
		}
		// Generate the texcoords for each individual subimage
		for (size_t i=0; i<subimage_amount; i++) {
			GLuint new_texcoord;
			GLfloat texcoords[] = {
				w*i,     0.0,
				w*(i+1), 0.0,
				w*(i+1), 1.0,
				w*i,     1.0,
			};
			glGenBuffers(1, &new_texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, new_texcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

			vbo_texcoords.push_back(new_texcoord); // Add the texcoord to the list of subimages
		}

		return 0; // Return 0 on success
	}
	/*
	* Sprite::crop_image() - Set a rectangle specifying the cropped size and generate a OpenGL buffers for it if necessary
	* ! Currently cropped images only support a single subimage
	* @new_crop: the rectangle to crop the image to
	*/
	int Sprite::crop_image(SDL_Rect new_crop) {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		crop = new_crop; // Set the crop properties

		// Reset the subimage properties if the image is being uncropped
		if ((crop.w == -1)&&(crop.h == -1)) {
			set_subimage_amount(1, width);
			crop = {0, 0, static_cast<int>(width), static_cast<int>(height)};
			return 0;
		}

		// Set the subimage properties to the crop properties
		set_subimage_amount(1, crop.w);
		subimages[0] = crop;

		if (get_options().renderer_type == E_RENDERER::SDL) { // If SDL rendering is being used, exit early
			return 0; // Return 0 on success
		}

		// Destroy all old texcoords
		if (!vbo_texcoords.empty()) {
			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();
		}

		// Convert the width and height of the crop rectangle to a percentage of the full texture dimensions
		GLfloat x = GLfloat(crop.x);
		GLfloat y = GLfloat(crop.y);
		GLfloat w = GLfloat(crop.w);
		GLfloat h = GLfloat(crop.h);
		x /= width; w /= width;
		y /= height; h /= height;

		// Generate the texcoords
		GLuint new_texcoord;
		GLfloat texcoords[] = {
			x,   y,
			x+w, y,
			x+w, y+h,
			x,   y+h
		};
		glGenBuffers(1, &new_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, new_texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

		vbo_texcoords.push_back(new_texcoord); // Add the texcoord to the list of subimages

		return 0; // Return 0 on success
	}
	/*
	* Sprite::crop_image_width() - Crop the image to the given width
	* @new_crop_width: the new width to crop the image to
	*/
	int Sprite::crop_image_width(int new_crop_width) {
		crop.w = new_crop_width;
		return crop_image(crop); // Return the status of the cropping
	}
	/*
	* Sprite::crop_image_height() - Crop the image to the given height
	* @new_crop_height: the new height to crop the image to
	*/
	int Sprite::crop_image_height(int new_crop_height) {
		crop.h = new_crop_height;
		return crop_image(crop); // Return the status of the cropping
	}

	/*
	* Sprite::load_from_surface() - Load a texture from the given surface
	* @tmp_surface: the temporary surface to load from
	*/
	int Sprite::load_from_surface(SDL_Surface* tmp_surface) {
		if (is_loaded) { // If the sprite has already been loaded, output a warning
			messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to load sprite \"" + name + "\" from surface because it has already been loaded");
			return 1; // Return 1 when not loaded
		}

		// Set the sprite dimensions
		width = tmp_surface->w;
		height = tmp_surface->h;

		// Generate the subimage buffers and dimensions
		if (subimage_amount <= 1) {
			set_subimage_amount(1, width); // If there are no subimages, treat the entire image as a single subimage
		} else {
			set_subimage_amount(subimage_amount, width/subimage_amount);
		}
		crop = {0, 0, static_cast<int>(width), static_cast<int>(height)}; // Set the default crop to be the entire image

		if (get_options().renderer_type != E_RENDERER::SDL) {
			// Generate the vertex array object for the sprite
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// Generate the four corner vertices of the rectangular sprite
			GLfloat vertices[] = {
				0.0,                                  0.0,
				static_cast<GLfloat>(subimage_width), 0.0,
				static_cast<GLfloat>(subimage_width), static_cast<GLfloat>(height),
				0.0,                                  static_cast<GLfloat>(height),
			};
			glGenBuffers(1, &vbo_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Generate the indices of the two triangles which form the rectangular sprite
			GLushort elements[] = {
				0, 1, 2,
				2, 3, 0,
			};
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

			// Bind the vertices to the VAO's vertex buffer
			glEnableVertexAttribArray(engine->renderer->vertex_location);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glVertexAttribPointer(
				engine->renderer->vertex_location,
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

			glBindVertexArray(0); // Unbind VAO when done loading
		} else {
			// Generate an SDL texture from the surface pixels
			texture = SDL_CreateTextureFromSurface(engine->renderer->sdl_renderer, tmp_surface);
			if (texture == nullptr) { // If the texture could not be generated, output a warning
				messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to create texture from surface for \"" + name + "\": " + get_sdl_error());
				return 2; // Return 2 on failure to create the texture
			}

			// Reset the blend mode and alpha to normal defaults
			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(texture, 255);
		}

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;

		return 0; // Return 0 on success
	}
	/*
	* Sprite::load() - Load the sprite from its given filename
	*/
	int Sprite::load() {
		if (is_loaded) { // Do not attempt to load the sprite if it has already been loaded
		       messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to load sprite \"" + name + "\" because it has already been loaded");
		       return 1; // Return 1 when already loaded
		}

		if (get_options().is_headless) {
			return 2; // Return 2 when in headless mode
		}

		// Load the sprite into a temporary surface
		SDL_Surface* tmp_surface;
		tmp_surface = IMG_Load(path.c_str());
		if (tmp_surface == nullptr) { // If the surface could not be loaded, output a warning
			messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to load sprite \"" + name + "\": " + IMG_GetError());
			return 3; // Return 3 on loding failure
		}

		load_from_surface(tmp_surface); // Load the surface into a texture
		SDL_FreeSurface(tmp_surface); // Free the temporary surface

		return 0; // Return 0 on success
	}
	/*
	* Sprite::free() - Free the sprite texture and delete all of its buffers
	*/
	int Sprite::free() {
		if (!is_loaded) { // Do not attempt to free the textures if the sprite has not been loaded
			return 0; // Return 0 on success
		}

		if (get_options().renderer_type != E_RENDERER::SDL) {
			// Delete the vertex and index buffer
			glDeleteBuffers(1, &vbo_vertices);
			glDeleteBuffers(1, &ibo);

			// Delete the texture coordinate buffers for each subimage
			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();

			// Delete the texture and the optional framebuffer
			glDeleteTextures(1, &gl_texture);
			glDeleteFramebuffers(1, &framebuffer);

			// Finally, delete the VAO
			glDeleteVertexArrays(1, &vao);
		} else {
			// Delete the SDL texture
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}

		// Reset the loaded booleans
		is_loaded = false;
		has_draw_failed = false;

		return 0; // Return 0 on success
	}
	/*
	* Sprite::drawing_begin() - Enable all required buffers
	*/
	int Sprite::drawing_begin() {
		if (get_options().renderer_type == E_RENDERER::SDL) {
			return 0; // Return 0 since nothing needs to be done for SDL mode
		}

		glBindVertexArray(vao); // Bind the VAO for the sprite

		// Bind the sprite texture
		glUniform1i(engine->renderer->texture_location, 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		return 0; // Return 0 on success
	}
	/*
	* Sprite::drawing_end() - Disable all required buffers
	*/
	int Sprite::drawing_end() {
		if (get_options().renderer_type == E_RENDERER::SDL) {
			return 0; // Return 0 since nothing needs to be done for SDL mode
		}

		glUniformMatrix4fv(engine->renderer->model_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the partial transformation matrix
		glUniformMatrix4fv(engine->renderer->rotation_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the rotation matrix
		glUniform1i(engine->renderer->flip_location, 0); // Reset the flip type

		glBindVertexArray(0); // Unbind the VAO

		return 0; // Return 0 on success
	}

	/*
	* Sprite::draw_subimage() - Draw a given subimage of the sprite with the given attributes
	* @x: the x-coordinate to draw the subimage at
	* @y: the y-coordinate to draw the subimage at
	* @current_subimage: the subimage of the sprite to draw
	* @w: the width to scale the subimage to
	* @h: the height to scale the subimage to
	* @angle: the number of degrees to rotate the subimage clockwise
	* @new_color: the color to paint the subimage in
	* @flip: the type of flip to draw the subimage with
	*/
	int Sprite::draw_subimage(int x, int y, unsigned int current_subimage, int w, int h, double angle, RGBA new_color, SDL_RendererFlip flip) {
		if (!is_loaded) { // Do not attempt to draw the subimage if it has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to draw sprite \"" + name + "\" because it is not loaded");
				has_draw_failed = true; // Set the draw failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		SDL_Rect drect = {x, y, 0, 0}; // Create a rectangle to define the position and dimensions of the destination render

		// Determine the desired width and height of the render
		if ((w >= 0)&&(h >= 0)) { // If the width and height are provided to the function, use them as is
			drect.w = w;
			drect.h = h;
		} else { // Otherwise set the width and height to the same as the sprite, i.e. don't scale the render
			drect.w = width;
			drect.h = height;
			if (subimage_amount > 1) {
				drect.w = subimage_width;
			}
		}

		if (get_options().renderer_type != E_RENDERER::SDL) {
			// Get the full width of the sprite to be used for scaling
			int rect_width = width;
			if (subimage_amount > 1) {
				rect_width = subimage_width;
			}

			// If the scaled width and height are not provided to the function, set them so that scaling will have no effect
			if (w <= 0) {
				w = rect_width;
			}
			if (h <= 0) {
				h = height;
			}

			drawing_begin();

			// Generate the partial transformation matrix (translation and scaling) for the subimage
			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(drect.x, drect.y, -0.5f)); // Translate the subimage the desired amount in the x- and y-planes, note that the z-coordinate is nonzero so that there is no z-fighting with backgrounds in 3D mode
			model = glm::scale(model, glm::vec3(static_cast<float>(w)/rect_width, static_cast<float>(h)/height, 1.0f)); // Scale the subimage in the x- and y-planes
			glUniformMatrix4fv(engine->renderer->model_location, 1, GL_FALSE, glm::value_ptr(model)); // Send the transformation matrix to the shader

			// Generate the rotation matrix for the subimage
			// This is not included in the above transformation matrix because it is faster to rotate everything in the geometry shader
			if (angle != 0.0) {
				glm::mat4 rotation = glm::translate(glm::mat4(1.0f), glm::vec3(rotate_x*rect_width, rotate_y*height, 0.0f));
				rotation = glm::rotate(rotation, static_cast<float>(degtorad(angle)), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate the subimage on the z-axis around the sprite's rotation origin at (rotate_x, rotate_y)
				rotation = glm::translate(rotation, glm::vec3(-rotate_x*rect_width, -rotate_y*height, 0.0f));
				glUniformMatrix4fv(engine->renderer->rotation_location, 1, GL_FALSE, glm::value_ptr(rotation)); // Send the rotation matrix to the shader
			}

			// Colorize the sprite with the given color
			glm::vec4 color = glm::vec4(new_color.r, new_color.g, new_color.b, new_color.a); // Normalize the color values from 0.0 to 1.0
			color /= 255.0f;
			glUniform4fv(engine->renderer->colorize_location, 1, glm::value_ptr(color)); // Send the color to the shader

			// Determine the desired flip type
			int f = 0; // The default behavior is to not flip
			if (flip & SDL_FLIP_HORIZONTAL) {
				f += 1;
			}
			if (flip & SDL_FLIP_VERTICAL) {
				f += 2;
			}
			glUniform1i(engine->renderer->flip_location, f); // Send the flip type to the shader

			// Add the subimage to the list of lightables so that it can cast shadows
			if (is_lightable) { // If the sprite is set as lightable
				// Fill a lightable data struct with the position and vertices of the subimage
				LightableData* l = new LightableData();
				l->position = glm::vec4(drect.x, drect.y, 0.0f, 0.0f);
				l->mask.reserve(4);
				l->mask.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
				l->mask.push_back(glm::vec4(rect_width, 0.0f, 0.0f, 0.0f));
				l->mask.push_back(glm::vec4(rect_width, height, 0.0f, 0.0f));
				l->mask.push_back(glm::vec4(0.0f, height, 0.0f, 0.0f));

				get_current_room()->add_lightable(l); // Add the struct to the room's list of lightables
			}

			// Bind the texture coordinates of the current subimage
			glEnableVertexAttribArray(engine->renderer->fragment_location);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords[current_subimage]);
			glVertexAttribPointer(
				engine->renderer->fragment_location,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
			);

			// Draw the triangles which form the rectangular subimage
			int size;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
			glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

			drawing_end();
		} else { // Otherwise, render with SDL
			if (is_on_screen(drect)) { // If the render will actually appear on screen
				// Colorize the sprite with the given color
				SDL_SetTextureColorMod(texture, new_color.r, new_color.g, new_color.b);
				SDL_SetTextureAlphaMod(texture, new_color.a);

				// Reset the sprite's blend mode to the global mode
				SDL_SetTextureBlendMode(texture, draw_get_blend());

				SDL_Point r = {static_cast<int>(rotate_x*subimage_width), static_cast<int>(rotate_y*height)}; // Create a point to use as the rotation origin
				// Render the subimage
				if (!subimages.empty()) { // If the sprite has multiple subimages, render without further cropping
					SDL_RenderCopyEx(engine->renderer->sdl_renderer, texture, &subimages[current_subimage], &drect, angle, &r, flip);
				} else { // Otherwise, render and crop as requested
					SDL_RenderCopyEx(engine->renderer->sdl_renderer, texture, &crop, &drect, angle, &r, flip);
				}
			}
		}

		// If the sprite has reached the end of its subimage cycle, set the animation boolean
		if ((is_animated)&&(current_subimage == subimage_amount-1)) {
			is_animated = false;
		}

		return 0; // Return 0 on success
	}
	/*
	* Sprite::draw() - Draw the sprite with a given subimage timing using the given attributes
	* @x: the x-coordinate to draw the sprite at
	* @y: the y-coordinate to draw the sprite at
	* @subimage_time: the frame of animation to choose the subimage from
	* @w: the width to scale the sprite to
	* @h: the height to scale the sprite to
	* @angle: the number of degrees to rotate the sprite clockwise
	* @new_color: the color to paint the sprite in
	* @flip: the type of flip to draw the sprite with
	*/
	int Sprite::draw(int x, int y, Uint32 subimage_time, int w, int h, double angle, RGBA new_color, SDL_RendererFlip flip) {
		// Calculate the current subimage to draw from the given animation frame
		unsigned int current_subimage = static_cast<unsigned int>(round(speed*(get_ticks()-subimage_time)/engine->fps_goal)) % subimage_amount;
		if (current_subimage == 0) { // If the first frame is being drawn, set the animation boolean
			is_animated = true;
		}

		return draw_subimage(x, y, current_subimage, w, h, angle, new_color, flip); // Return the result of drawing the subimage
	}
	/*
	* Sprite::draw() - Draw the sprite with a given subimage timing using the given attributes
	* ! When the function is called with no other attributes, simply call it with values that will not affect the render
	* @x: the x-coordinate to draw the sprite at
	* @y: the y-coordinate to draw the sprite at
	* @subimage_time: the frame of animation to choose the subimage from
	*/
	int Sprite::draw(int x, int y, Uint32 subimage_time) {
		return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}, SDL_FLIP_NONE); // Return the result of drawing the sprite
	}
	/*
	* Sprite::draw() - Draw the sprite with a given subimage timing using the given attributes
	* ! When the function is called with only width and height, simply call it with values that will not affect the other parts of the render
	* @x: the x-coordinate to draw the sprite at
	* @y: the y-coordinate to draw the sprite at
	* @subimage_time: the frame of animation to choose the subimage from
	* @w: the width to scale the sprite to
	* @h: the height to scale the sprite to
	*/
	int Sprite::draw(int x, int y, Uint32 subimage_time, int w, int h) {
		return draw(x, y, subimage_time, w, h, 0.0, {255, 255, 255, 255}, SDL_FLIP_NONE); // Return the result of drawing the scaled sprite
	}
	/*
	* Sprite::draw() - Draw the sprite with a given subimage timing using the given attributes
	* ! When the function is called with only the rotation angle, simply call it with values that will not affect the other parts of the render
	* @x: the x-coordinate to draw the sprite at
	* @y: the y-coordinate to draw the sprite at
	* @subimage_time: the frame of animation to choose the subimage from
	* @angle: the number of degrees to rotate the sprite clockwise
	*/
	int Sprite::draw(int x, int y, Uint32 subimage_time, double angle) {
		return draw(x, y, subimage_time, -1, -1, angle, {255, 255, 255, 255}, SDL_FLIP_NONE); // Return the result of drawing the rotated sprite
	}
	/*
	* Sprite::draw() - Draw the sprite with a given subimage timing using the given attributes
	* ! When the function is called with only the colorization color, simply call it with values that will not affect the other parts of the render
	* @x: the x-coordinate to draw the sprite at
	* @y: the y-coordinate to draw the sprite at
	* @subimage_time: the frame of animation to choose the subimage from
	* @color: the color to paint the sprite in
	*/
	int Sprite::draw(int x, int y, Uint32 subimage_time, RGBA color) {
		return draw(x, y, subimage_time, -1, -1, 0.0, color, SDL_FLIP_NONE); // Return the result of drawing the colorized sprite
	}
	/*
	* Sprite::draw() - Draw the sprite with a given subimage timing using the given attributes
	* ! When the function is called with only the flip type, simply call it with values that will not affect the other parts of the render
	* @x: the x-coordinate to draw the sprite at
	* @y: the y-coordinate to draw the sprite at
	* @subimage_time: the frame of animation to choose the subimage from
	* @flip: the type of flip to draw the sprite with
	*/
	int Sprite::draw(int x, int y, Uint32 subimage_time, SDL_RendererFlip flip) {
		return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}, flip); // Return the result of drawing the flipped sprite
	}
	/*
	* Sprite::draw_simple() - Draw the sprite with a simple SDL blit
	* ! Note that this will only succeed in SDL rendering mode
	* @source: the source rectangle on the sprite, i.e. the desired cropping
	* @dest: the destination rectangle on the canvas, i.e. the desired scaling
	*/
	int Sprite::draw_simple(SDL_Rect* source, SDL_Rect* dest) {
		if (!is_loaded) { // If the sprite is not loaded, exit
			return 1; // Return 1 when not loaded
		}
		if (get_options().renderer_type != E_RENDERER::SDL) { // If the rendering mode is not SDL, exit
			return 2; // Return 2 when not in SDL rendering more
		}

		return SDL_RenderCopy(engine->renderer->sdl_renderer, texture, source, dest); // Return the result of rendering the sprite
	}
	/*
	* Sprite::draw_array() - Draw a list of sprite instances with the given attributes
	* ! This is primarily used by the engine particle rendering system, however it is not certain whether this actually provides any speedups
	* @draw_list: the list of sprite instances to draw
	* @rotation_cache: a list of precalculated rotation matrices for the desired degrees of rotation
	* @new_color: the color to paint the sprites in
	* @flip: the type of flip to draw the sprites with
	*/
	int Sprite::draw_array(const std::list<SpriteDrawData*>& draw_list, const std::vector<glm::mat4>& rotation_cache, RGBA new_color, SDL_RendererFlip flip) {
		if (!is_loaded) { // Do not attempt to draw the instances if the sprite is not loaded
			if (!has_draw_failed) { // If the draw call hasn't failed yet, output a warning
				messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to draw sprite instances for \"" + name + "\" because it is not loaded");
				has_draw_failed = true; // Set the draw failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		if (get_options().renderer_type != E_RENDERER::SDL) {
			drawing_begin();

			// Colorize the sprite with the given color
			glm::vec4 color = glm::vec4(new_color.r, new_color.g, new_color.b, new_color.a); // Normalize the color values from 0.0 to 1.0
			color /= 255.0f;
			glUniform4fv(engine->renderer->colorize_location, 1, glm::value_ptr(color)); // Send the color to the shader

			// Determine the desired flip type
			int f = 0; // The default behavior is to not flip
			if (flip & SDL_FLIP_HORIZONTAL) {
				f += 1;
			}
			if (flip & SDL_FLIP_VERTICAL) {
				f += 2;
			}
			glUniform1i(engine->renderer->flip_location, f); // Send the flip type to the shader

			// Get the amount indices that will be drawn per sprite instance
			int size;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

			LightableData* lightable_data = new LightableData();
			lightable_data->mask.reserve(4);
			lightable_data->mask.push_back(glm::vec4());
			lightable_data->mask.push_back(glm::vec4());
			lightable_data->mask.push_back(glm::vec4());
			lightable_data->mask.push_back(glm::vec4());

			Uint32 t = get_ticks(); // Calculate the current time to be used when finding the subimage to draw
			for (auto& s : draw_list) { // Loop over the list and draw each instance
				int current_subimage = static_cast<int>(round(speed*(t-s->subimage_time)/get_fps_goal())) % subimage_amount; // Calculate the current subimage of each instace from the given animation frame

				SDL_Rect drect = {s->x, s->y, 0, 0}; // Create a rectangle to define the position and dimensions of the destination render

				// Determine the desired width and height of the render
				if ((s->w >= 0)&&(s->h >= 0)) { // If the width and height are provided to the function, use them as is
					drect.w = s->w;
					drect.h = s->h;
				} else { // Otherwise set the width and height to the same as the sprite, i.e. don't scale the render
					drect.w = width;
					drect.h = height;
					if (subimage_amount > 1) {
						drect.w = subimage_width;
					}
				}

				// Get the full width of the sprite to be used for scaling
				int rect_width = width;
				if (subimage_amount > 1) {
					rect_width = subimage_width;
				}

				// If the scaled width and height are not provided to the function, set them so that scaling will have no effect
				if (s->w <= 0) {
					s->w = rect_width;
				}
				if (s->h <= 0) {
					s->h = height;
				}

				// Generate the partial transformation matrix (translation and scaling) for the instance
				glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(drect.x, drect.y, -1.0f)); // Translate the instance the desired amount in the x- and y-planes, note that the z-coordinate is nonzero so that there is no z-fighting with non-particle sprites in 3D mode
				model = glm::scale(model, glm::vec3(static_cast<float>(s->w)/rect_width, static_cast<float>(s->h)/height, 1.0)); // Scale the instance in the x- and y-planes
				glUniformMatrix4fv(engine->renderer->model_location, 1, GL_FALSE, glm::value_ptr(model)); // Send the transformation matrix to the shader

				// Send the cached rotation matrix to the shader
				// This is not included in the above transformation matrix because it is faster to rotate everything in the geometry shader
				if (s->angle != 0.0) {
					glUniformMatrix4fv(engine->renderer->rotation_location, 1, GL_FALSE, glm::value_ptr(rotation_cache[static_cast<unsigned int>(s->angle)]));
				}

				// Add the subimage to the list of lightables so that it can cast shadows
				if (is_lightable) { // If the sprite is set as lightable
					// Fill a lightable data struct with the position and vertices of the subimage
					LightableData* l = new LightableData(*lightable_data);
					l->position = glm::vec4(drect.x, drect.y, 0.0f, 0.0f);
					l->mask[1].x = static_cast<float>(rect_width);
					l->mask[2].x = static_cast<float>(rect_width);
					l->mask[2].y = static_cast<float>(height);
					l->mask[3].y = static_cast<float>(height);

					get_current_room()->add_lightable(l); // Add the struct to the room's list of lightables
				}

				// Bind the texture coordinates of the current subimage
				glEnableVertexAttribArray(engine->renderer->fragment_location);
				glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords[current_subimage]);
				glVertexAttribPointer(
					engine->renderer->fragment_location,
					2,
					GL_FLOAT,
					GL_FALSE,
					0,
					0
				);

				glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0); // Draw the triangles which form the rectangular subimage
			}

			delete lightable_data;

			drawing_end();
		} else {
			// Colorize the instances with the given color
			SDL_SetTextureColorMod(texture, new_color.r, new_color.g, new_color.b);
			SDL_SetTextureAlphaMod(texture, new_color.a);

			Uint32 t = get_ticks(); // Calculate the current time to be used when finding the subimage to draw
			for (auto& s : draw_list) { // Loop over the list and draw each instance
				int current_subimage = static_cast<int>(round(speed*(t-s->subimage_time)/get_fps_goal())) % subimage_amount; // Calculate the current subimage of each instace from the given animation frame

				SDL_Rect drect = {s->x, s->y, 0, 0}; // Create a rectangle to define the position and dimensions of the destination render

				// Determine the desired width and height of the render
				if ((s->w >= 0)&&(s->h >= 0)) { // If the width and height are provided to the function, use them as is
					drect.w = s->w;
					drect.h = s->h;
				} else { // Otherwise set the width and height to the same as the sprite, i.e. don't scale the render
					drect.w = width;
					drect.h = height;
					if (subimage_amount > 1) {
						drect.w = subimage_width;
					}
				}

				// Render the subimage
				if (!subimages.empty()) { // If the sprite has multiple subimages, render without further cropping
					SDL_RenderCopyEx(engine->renderer->sdl_renderer, texture, &subimages[current_subimage], &drect, s->angle, nullptr, flip);
				} else { // Otherwise, render without cropping
					SDL_RenderCopyEx(engine->renderer->sdl_renderer, texture, nullptr, &drect, s->angle, nullptr, flip);
				}
			}
		}

		return 0; // Return 0 on success
	}
	/*
	* Sprite::set_as_target() - Set up the sprite for use as a render target
	* @w: the width of the render texture
	* @h: the height of the render texture
	*/
	int Sprite::set_as_target(int w, int h) {
		if (is_loaded) { // If the sprite is already loaded, free the old data
			this->free();
		}

		if (get_options().is_headless) {
			return -1; // Return -1 when in headless mode
		}

		// Set the sprite dimensions and remove all cropping
		width = w;
		height = h;
		set_subimage_amount(1, width);
		crop = {0, 0, static_cast<int>(width), static_cast<int>(height)};

		if (get_options().renderer_type != E_RENDERER::SDL) {
			// Generate the vertex array object for the sprite
			glGenVertexArrays(1, &vao);
			glBindVertexArray(vao);

			// Generate the four corner vertices of the rectangular sprite
			GLfloat vertices[] = {
				0.0,                         0.0,
				static_cast<GLfloat>(width), 0.0,
				static_cast<GLfloat>(width), static_cast<GLfloat>(height),
				0.0,                         static_cast<GLfloat>(height),
			};
			glGenBuffers(1, &vbo_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			// Generate the indices of the two triangles which form the rectangular sprite
			GLushort elements[] = {
				0, 1, 2,
				2, 3, 0,
			};
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

			// Bind the vertices to the VAO's vertex buffer
			glEnableVertexAttribArray(engine->renderer->vertex_location);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glVertexAttribPointer(
				engine->renderer->vertex_location,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
			);

			// Generate the framebuffer that will be used to render to this sprite
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
				messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to create a new framebuffer.");
				glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer to switch back to the default
				this->free(); // Free the old data
				return 0; // Return 0 on failure to initialize the framebuffer
			}

			glBindVertexArray(0); // Unbind VAO when done loading

			// Set the loaded booleans
			is_loaded = true;
			has_draw_failed = false;

			return static_cast<int>(framebuffer); // Return the framebuffer index on success
		} else {
			texture = SDL_CreateTexture(engine->renderer->sdl_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h); // Create an empty texture
			if (texture == nullptr) { // If the texture could not be created, output a warning
				messenger::send({"engine", "sprite"}, E_MESSAGE::WARNING, "Failed to create a blank texture: " + get_sdl_error());
				return 0; // Return 0 on failure to create a blank texture
			}

			SDL_SetRenderTarget(engine->renderer->sdl_renderer, texture); // Set the SDL render target
		}

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;

		return 1; // Return a positive integer on success
	}
	/*
	* Sprite::set_as_target() - Set up the sprite for use as a render target
	* ! When the function is called without specifying width and height, simply call it with the window dimensions
	*/
	int Sprite::set_as_target() {
		return set_as_target(bee::get_width(), bee::get_height()); // Return the result of attempting to set the render target
	}
}

#endif // BEE_SPRITE
