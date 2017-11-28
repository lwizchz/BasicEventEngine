/*
* Copyright (c) 2015-17 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_TEXTURE
#define BEE_TEXTURE 1

#include "../defines.hpp"

#include <sstream> // Include the required library headers

#include <SDL2/SDL_image.h> // Include the required SDL headers

#include <glm/gtc/matrix_transform.hpp> // Include the required OpenGL headers
#include <glm/gtc/type_ptr.hpp>

#include "texture.hpp" // Include the class resource header

#include "../engine.hpp"

#include "../util/real.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/enginestate.hpp"
#include "../core/rooms.hpp"
#include "../core/window.hpp"

#include "../render/drawing.hpp"
#include "../render/render.hpp"
#include "../render/renderer.hpp"
#include "../render/shader.hpp"

#include "light.hpp"
#include "room.hpp"

namespace bee {
	TextureDrawData::TextureDrawData(GLuint _vao, GLuint _texture, GLuint _ibo) :
		TextureDrawData(_vao, _texture, _ibo, glm::mat4(1.0f), glm::mat4(1.0f), glm::vec4(1.0f), -1)
	{}
	TextureDrawData::TextureDrawData(GLuint _vao, GLuint _texture, GLuint _ibo, glm::mat4 _model, glm::mat4 _rotation, glm::vec4 _color, GLuint _buffer) :
		vao(_vao),
		texture(_texture),
		ibo(_ibo),

		model(_model),
		rotation(_rotation),
		color(_color),
		buffer(_buffer)
	{}

	std::map<int,Texture*> Texture::list;
	int Texture::next_id = 0;

	/*
	* Texture::Texture() - Default construct the texture
	* ! This constructor should only be directly used for temporary textures (e.g. framebuffers), the other constructor should be used for all other cases
	*/
	Texture::Texture() :
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

		vao(-1),
		vbo_vertices(-1),
		ibo(-1),
		gl_texture(-1),
		vbo_texcoords(),

		framebuffer(-1)
	{}
	/*
	* Texture::Texture() - Construct the texture, add it to the texture resource list, and set the new name and path
	* @new_name: the name of the texture to use
	* @new_path: the path of the texture's image
	*/
	Texture::Texture(const std::string& new_name, const std::string& new_path) :
		Texture() // Default initialize all variables
	{
		if (add_to_resources() < 0) { // Attempt to add the texture to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::ERROR, "Failed to add texture resource: \"" + new_name + "\" from " + new_path);
			throw(-1); // Throw an exception if it could not be added
		}

		set_name(new_name);
		set_path(new_path);
	}
	/*
	* Texture::~Texture() - Free the texture data and remove it from the resource list
	*/
	Texture::~Texture() {
		this->free(); // Free all texture data
		list.erase(id); // Remove the texture from the resource list
	}

	/*
	* Texture::add_to_resources() - Add the texture to the appropriate resource list
	*/
	int Texture::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = next_id++;
			list.emplace(id, this); // Add the resource and with the new id
		}

		return id; // Return the id on success
	}
	/*
	* Texture::get_amount() - Return the amount of texture resources
	*/
	size_t Texture::get_amount() {
		return list.size();
	}
	/*
	* Texture::get() - Return the resource with the given id
	* @id: the resource to get
	*/
	Texture* Texture::get(int id) {
		if (list.find(id) != list.end()) {
			return list[id];
		}
		return nullptr;
	}
	/*
	* Texture::reset() - Reset all resource variables for reinitialization
	*/
	int Texture::reset() {
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
		has_draw_failed = false;

		return 0; // Return 0 on success
	}
	/*
	* Texture::print() - Print all relevant information about the resource
	*/
	int Texture::print() const {
		std::stringstream s; // Declare the output stream
		s << // Append all info to the output
		"Texture { "
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
		"\n}\n";
		messenger::send({"engine", "resource"}, E_MESSAGE::INFO, s.str()); // Send the info to the messaging system for output

		return 0; // Return 0 on success
	}

	/*
	* Texture::get_*() - Return the requested resource information
	*/
	int Texture::get_id() const {
		return id;
	}
	std::string Texture::get_name() const {
		return name;
	}
	std::string Texture::get_path() const {
		return path;
	}
	int Texture::get_width() const {
		return width;
	}
	int Texture::get_height() const {
		return height;
	}
	int Texture::get_subimage_amount() const {
		return subimage_amount;
	}
	int Texture::get_subimage_width() const {
		return subimage_width;
	}
	double Texture::get_speed() const {
		return speed;
	}
	bool Texture::get_is_animated() const {
		return is_animated;
	}
	int Texture::get_origin_x() const {
		return origin_x;
	}
	int Texture::get_origin_y() const {
		return origin_y;
	}
	double Texture::get_rotate_x() const {
		return rotate_x;
	}
	double Texture::get_rotate_y() const {
		return rotate_y;
	}
	SDL_Texture* Texture::get_texture() const {
		return texture;
	}
	bool Texture::get_is_loaded() const {
		return is_loaded;
	}

	/*
	* Texture::set_*() - Set the requested resource data
	*/
	int Texture::set_name(const std::string& new_name) {
		name = new_name;
		return 0;
	}
	int Texture::set_path(const std::string& new_path) {
		path = "resources/textures/"+new_path; // Append the path to the texture directory
		return 0;
	}
	int Texture::set_speed(double new_speed) {
		speed = new_speed;
		return 0;
	}
	int Texture::set_origin_xy(int new_origin_x, int new_origin_y) {
		origin_x = new_origin_x;
		origin_y = new_origin_y;
		return 0;
	}
	int Texture::set_origin_x(int new_origin_x) {
		return set_origin_xy(new_origin_x, get_origin_y());
	}
	int Texture::set_origin_y(int new_origin_y) {
		return set_origin_xy(get_origin_x(), new_origin_y);
	}
	int Texture::set_origin_center() {
		return set_origin_xy(subimage_width/2, height/2);
	}
	/*
	* Texture::set_rotate_xy() - Set both coordinates of the rotation origin
	* ! Note that all x- and y-coordinates of the rotation origin are given as a percentage of the width and height of the image
	* @new_rotate_x: the new x-coordinate to rotate the texture around
	* @new_rotate_y: the new y-coordinate to rotate the texture around
	*/
	int Texture::set_rotate_xy(double new_rotate_x, double new_rotate_y) {
		rotate_x = new_rotate_x;
		rotate_y = new_rotate_y;
		return 0;
	}
	int Texture::set_rotate_x(double new_rotate_x) {
		return set_rotate_xy(new_rotate_x, get_rotate_y());
	}
	int Texture::set_rotate_y(double new_rotate_y) {
		return set_rotate_xy(get_rotate_x(), new_rotate_y);
	}
	int Texture::set_rotate_center() {
		return set_rotate_xy(0.5, 0.5);
	}
	/*
	* Texture::set_subimage_amount() - Set subimage coordinates and generate OpenGL buffers if necessary
	* @new_subimage_amount: the amount of subimages to use
	* @new_subimage_width: the width of each subimage
	*/
	int Texture::set_subimage_amount(int new_subimage_amount, int new_subimage_width) {
		if (get_options().is_headless) {
			return 1; // Return 1 when in headless mode
		}

		// Reset the subimage properties
		subimage_amount = new_subimage_amount;
		subimage_width = new_subimage_width;

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
	* Texture::crop_image() - Set a rectangle specifying the cropped size and generate a OpenGL buffers for it if necessary
	* ! Currently cropped images only support a single subimage
	* @new_crop: the rectangle to crop the image to
	*/
	int Texture::crop_image(SDL_Rect new_crop) {
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
	* Texture::crop_image_width() - Crop the image to the given width
	* @new_crop_width: the new width to crop the image to
	*/
	int Texture::crop_image_width(int new_crop_width) {
		crop.w = new_crop_width;
		return crop_image(crop); // Return the status of the cropping
	}
	/*
	* Texture::crop_image_height() - Crop the image to the given height
	* @new_crop_height: the new height to crop the image to
	*/
	int Texture::crop_image_height(int new_crop_height) {
		crop.h = new_crop_height;
		return crop_image(crop); // Return the status of the cropping
	}

	/*
	* Texture::load_from_surface() - Load a texture from the given surface
	* @tmp_surface: the temporary surface to load from
	*/
	int Texture::load_from_surface(SDL_Surface* tmp_surface) {
		if (is_loaded) { // If the texture has already been loaded, output a warning
			messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to load texture \"" + name + "\" from surface because it has already been loaded");
			return 1; // Return 1 when not loaded
		}

		// Set the texture dimensions
		width = tmp_surface->w;
		height = tmp_surface->h;

		// Generate the subimage buffers and dimensions
		if (subimage_amount <= 1) {
			set_subimage_amount(1, width); // If there are no subimages, treat the entire image as a single subimage
		} else {
			set_subimage_amount(subimage_amount, width/subimage_amount);
		}
		crop = {0, 0, static_cast<int>(width), static_cast<int>(height)}; // Set the default crop to be the entire image

		// Generate the vertex array object for the texture
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate the four corner vertices of the rectangular texture
		GLfloat vertices[] = {
			0.0,                                  0.0,
			static_cast<GLfloat>(subimage_width), 0.0,
			static_cast<GLfloat>(subimage_width), static_cast<GLfloat>(height),
			0.0,                                  static_cast<GLfloat>(height),
		};
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Generate the indices of the two triangles which form the rectangular texture
		GLushort elements[] = {
			0, 1, 2,
			2, 3, 0,
		};
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		// Bind the vertices to the VAO's vertex buffer
		glEnableVertexAttribArray(engine->renderer->program->get_location("v_position"));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(
			engine->renderer->program->get_location("v_position"),
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

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;

		return 0; // Return 0 on success
	}
	/*
	* Texture::load() - Load the texture from its given filename
	*/
	int Texture::load() {
		if (is_loaded) { // Do not attempt to load the texture if it has already been loaded
		       messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to load texture \"" + name + "\" because it has already been loaded");
		       return 1; // Return 1 when already loaded
		}

		if ((get_options().is_headless)||(get_options().renderer_type == E_RENDERER::SDL)) {
			return 2; // Return 2 when texture rendering is not applicable
		}

		// Load the texture into a temporary surface
		SDL_Surface* tmp_surface;
		tmp_surface = IMG_Load(path.c_str());
		if (tmp_surface == nullptr) { // If the surface could not be loaded, output a warning
			messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to load texture \"" + name + "\": " + IMG_GetError());
			return 3; // Return 3 on loding failure
		}

		load_from_surface(tmp_surface); // Load the surface into a texture
		SDL_FreeSurface(tmp_surface); // Free the temporary surface

		return 0; // Return 0 on success
	}
	/*
	* Texture::load_as_target() - Setup the texture for use as a render target
	* @w: the target width to use
	* @h: the target height to use
	*/
	int Texture::load_as_target(int w, int h) {
		if (is_loaded) { // Do not attempt to load the texture if it has already been loaded
		       messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to load texture \"" + name + "\" because it has already been loaded");
		       return 1; // Return 1 when already loaded
		}

		if ((get_options().is_headless)||(get_options().renderer_type == E_RENDERER::SDL)) {
			return 2; // Return 2 when texture rendering is not applicable
		}

		// Set the texture dimensions and remove all cropping
		width = w;
		height = h;
		set_subimage_amount(1, width);
		crop = {0, 0, static_cast<int>(width), static_cast<int>(height)};

		// Generate the vertex array object for the texture
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		// Generate the four corner vertices of the rectangular texture
		GLfloat vertices[] = {
			0.0,                         0.0,
			static_cast<GLfloat>(width), 0.0,
			static_cast<GLfloat>(width), static_cast<GLfloat>(height),
			0.0,                         static_cast<GLfloat>(height),
		};
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		// Generate the indices of the two triangles which form the rectangular texture
		GLushort elements[] = {
			0, 1, 2,
			2, 3, 0,
		};
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		// Bind the vertices to the VAO's vertex buffer
		glEnableVertexAttribArray(engine->renderer->program->get_location("v_position"));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(
			engine->renderer->program->get_location("v_position"),
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);

		// Generate the framebuffer that will be used to render to this texture
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
		glDrawBuffers(1, buffer); // Enable drawing to the framebuffer

		// Check whether the framebuffer has been successfully initialized
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) { // If not, reset the state
			messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to create a new framebuffer");
			glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind the frame buffer to switch back to the default
			this->free(); // Free the old data
			return 3; // Return 0 on failure to initialize the framebuffer
		}

		glBindVertexArray(0); // Unbind VAO when done loading

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;

		return 0; // Return 0 on success
	}
	/*
	* Texture::free() - Free the texture texture and delete all of its buffers
	*/
	int Texture::free() {
		if (!is_loaded) { // Do not attempt to free the textures if the texture has not been loaded
			return 0; // Return 0 on success
		}

		// Delete the vertex and index buffer
		glDeleteBuffers(1, &vbo_vertices);
		glDeleteBuffers(1, &ibo);
		vbo_vertices = -1;
		ibo = -1;

		// Delete the texture coordinate buffers for each subimage
		for (auto& t : vbo_texcoords) {
			glDeleteBuffers(1, &t);
		}
		vbo_texcoords.clear();

		// Delete the texture and the optional framebuffer
		glDeleteTextures(1, &gl_texture);
		glDeleteFramebuffers(1, &framebuffer);
		gl_texture = -1;
		framebuffer = -1;

		// Finally, delete the VAO
		glDeleteVertexArrays(1, &vao);
		vao = -1;

		// Reset the loaded booleans
		is_loaded = false;
		has_draw_failed = false;

		return 0; // Return 0 on success
	}
	/*
	* Texture::drawing_begin() - Enable all required buffers
	*/
	int Texture::drawing_begin() {
		glBindVertexArray(vao); // Bind the VAO for the texture

		// Bind the texture texture
		glUniform1i(engine->renderer->program->get_location("f_texture"), 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		return 0; // Return 0 on success
	}
	/*
	* Texture::drawing_end() - Disable all required buffers
	*/
	int Texture::drawing_end() {
		glUniformMatrix4fv(engine->renderer->program->get_location("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the partial transformation matrix
		glUniformMatrix4fv(engine->renderer->program->get_location("rotation"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the rotation matrix
		glUniform1i(engine->renderer->program->get_location("flip"), 0); // Reset the flip type

		glBindVertexArray(0); // Unbind the VAO

		return 0; // Return 0 on success
	}

	/*
	* Texture::draw_subimage() - Draw a given subimage of the texture with the given attributes
	* @x: the x-coordinate to draw the subimage at
	* @y: the y-coordinate to draw the subimage at
	* @current_subimage: the subimage of the texture to draw
	* @w: the width to scale the subimage to
	* @h: the height to scale the subimage to
	* @angle: the number of degrees to rotate the subimage clockwise
	* @new_color: the color to paint the subimage in
	*/
	int Texture::draw_subimage(int x, int y, unsigned int current_subimage, int w, int h, double angle, RGBA new_color) {
		if (!is_loaded) { // Do not attempt to draw the subimage if it has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to draw texture \"" + name + "\" because it is not loaded");
				has_draw_failed = true; // Set the draw failure boolean
			}
			return 1; // Return 1 when not loaded
		}

		SDL_Rect drect = {x, y, 0, 0}; // Create a rectangle to define the position and dimensions of the destination render

		// Determine the desired width and height of the render
		if ((w >= 0)&&(h >= 0)) { // If the width and height are provided to the function, use them as is
			drect.w = w;
			drect.h = h;
		} else { // Otherwise set the width and height to the same as the texture, i.e. don't scale the render
			drect.w = width;
			drect.h = height;
			if (subimage_amount > 1) {
				drect.w = subimage_width;
			}
		}

		// Get the full width of the texture to be used for scaling
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

		TextureDrawData td (vao, gl_texture, ibo);

		// Generate the partial transformation matrix (translation and scaling) for the subimage
		td.model = glm::translate(glm::mat4(1.0f), glm::vec3(drect.x, drect.y, -0.5f)); // Translate the subimage the desired amount in the x- and y-planes, note that the z-coordinate is nonzero so that there is no z-fighting with backgrounds in 3D mode
		td.model = glm::scale(td.model, glm::vec3(static_cast<float>(w)/rect_width, static_cast<float>(h)/height, 1.0f)); // Scale the subimage in the x- and y-planes

		// Generate the rotation matrix for the subimage
		// This is not included in the above transformation matrix because it is faster to rotate everything in the geometry shader
		if (angle != 0.0) {
			td.rotation = glm::translate(glm::mat4(1.0f), glm::vec3(rotate_x*rect_width, rotate_y*height, 0.0f));
			td.rotation = glm::rotate(td.rotation, static_cast<float>(degtorad(angle)), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate the subimage on the z-axis around the texture's rotation origin at (rotate_x, rotate_y)
			td.rotation = glm::translate(td.rotation, glm::vec3(-rotate_x*rect_width, -rotate_y*height, 0.0f));
		}

		// Colorize the texture with the given color
		td.color = glm::vec4(new_color.r, new_color.g, new_color.b, new_color.a); // Normalize the color values from 0.0 to 1.0
		td.color /= 255.0f;

		// Bind the texture coordinates of the current subimage
		td.buffer = vbo_texcoords[current_subimage];

		render::queue_texture(this, td);

		// If the texture has reached the end of its subimage cycle, set the animation boolean
		if ((is_animated)&&(current_subimage == subimage_amount-1)) {
			is_animated = false;
		}

		return 0; // Return 0 on success
	}
	/*
	* Texture::draw() - Draw the texture with a given subimage timing using the given attributes
	* @x: the x-coordinate to draw the texture at
	* @y: the y-coordinate to draw the texture at
	* @subimage_time: the frame of animation to choose the subimage from
	* @w: the width to scale the texture to
	* @h: the height to scale the texture to
	* @angle: the number of degrees to rotate the texture clockwise
	* @new_color: the color to paint the texture in
	*/
	int Texture::draw(int x, int y, Uint32 subimage_time, int w, int h, double angle, RGBA new_color) {
		// Calculate the current subimage to draw from the given animation frame
		unsigned int current_subimage = static_cast<unsigned int>(round(speed*(get_ticks()-subimage_time)/engine->fps_goal)) % subimage_amount;
		if (current_subimage == 0) { // If the first frame is being drawn, set the animation boolean
			is_animated = true;
		}

		return draw_subimage(x, y, current_subimage, w, h, angle, new_color); // Return the result of drawing the subimage
	}
	/*
	* Texture::draw() - Draw the texture with a given subimage timing using the given attributes
	* ! When the function is called with no other attributes, simply call it with values that will not affect the render
	* @x: the x-coordinate to draw the texture at
	* @y: the y-coordinate to draw the texture at
	* @subimage_time: the frame of animation to choose the subimage from
	*/
	int Texture::draw(int x, int y, Uint32 subimage_time) {
		return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}); // Return the result of drawing the texture
	}
	/*
	* Texture::set_as_target() - Set the texture as the render target
	*/
	GLuint Texture::set_as_target() {
		if (framebuffer == static_cast<GLuint>(-1)) {
			messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to set render target: uninitialized framebuffer");
			return -1;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// Bind the framebuffer to the empty texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_texture, 0);
		GLenum buffer[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, buffer); // Enable drawing to the framebuffer

		return framebuffer;
	}
}

#endif // BEE_TEXTURE
