/*
* Copyright (c) 2015-18 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef BEE_TEXTURE
#define BEE_TEXTURE 1

#include "texture.hpp" // Include the class resource header

#include <SDL2/SDL_image.h> // Include the required SDL headers

#include <glm/gtc/matrix_transform.hpp> // Include the required OpenGL headers
#include <glm/gtc/type_ptr.hpp>

#include "../engine.hpp"

#include "../util/real.hpp"
#include "../util/debug.hpp"

#include "../init/gameoptions.hpp"

#include "../messenger/messenger.hpp"

#include "../core/rooms.hpp"

#include "../render/render.hpp"
#include "../render/shader.hpp"

namespace bee {
	/**
	* Construct the data struct and initiliaze all values.
	*/
	TextureTransform::TextureTransform() :
		TextureTransform(0, 0, false, false, 0, 0, false)
	{}
	/**
	* Construct the data struct and initiliaze it with all the given values.
	* @param _x the top-left x-coordinate
	* @param _y the top-left y-coordinate
	* @param _is_horizontal_tile whether the Texture should be tiled horizontally
	* @param _is_vertical_tile whether the Texture should be tiled vertically
	* @param _horizontal_speed the speed at which the Texture should move horizontally in pixels per second
	* @param _vertical_speed the speed with which the Texture should move vertically in pixels per second
	* @param _is_stretched whether to stretch the Texture to fit the screen, which will cause some of the above values to be ignored
	*/
	TextureTransform::TextureTransform(int _x, int _y, bool _is_horizontal_tile, bool _is_vertical_tile, int _horizontal_speed, int _vertical_speed, bool _is_stretched) :
		x(_x),
		y(_y),
		is_horizontal_tile(_is_horizontal_tile),
		is_vertical_tile(_is_vertical_tile),
		horizontal_speed(_horizontal_speed),
		vertical_speed(_vertical_speed),
		is_stretched(_is_stretched)
	{}

	/**
	* Construct the data struct and initialize the given values.
	* @param _vao the Vertex Array %Object
	* @param _texture the OpenGL texture
	* @param _ibo the Index Buffer %Object
	*/
	TextureDrawData::TextureDrawData(GLuint _vao, GLuint _texture, GLuint _ibo) :
		TextureDrawData(_vao, _texture, _ibo, glm::mat4(1.0f), glm::mat4(1.0f), glm::vec4(1.0f), -1)
	{}
	/**
	* Construct the data struct and initialize it with the given values.
	* @param _vao the Vertex Array %Object
	* @param _texture the OpenGL texture
	* @param _ibo the Index Buffer %Object
	* @param _model the model transform
	* @param _rotation the rotational transform
	* @param _color the colorization to use
	* @param _buffer the texcoord buffer
	*/
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

	/**
	* Default construct the Texture.
	* @note This constructor should only be directly used for temporary Textures, e.g. framebuffers.
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
		speed(1.0),
		is_animated(false),
		origin({0, 0}),
		rotate({0.5, 0.5}),

		is_loaded(false),
		texture(nullptr),
		has_draw_failed(false),

		vao(-1),
		vbo_vertices(-1),
		ibo(-1),
		gl_texture(-1),
		vbo_texcoords(),

		framebuffer(-1)
	{}
	/**
	* Construct the Texture, add it to the Texture resource list, and set the new name and path.
	* @param _name the name of the Texture to use
	* @param _path the path of the Texture's image
	*/
	Texture::Texture(const std::string& _name, const std::string& _path) :
		Texture() // Default initialize all variables
	{
		if (add_to_resources() < 0) { // Attempt to add the Texture to its resource list
			messenger::send({"engine", "resource"}, E_MESSAGE::ERROR, "Failed to add Texture resource: \"" + _name + "\" from " + _path);
			throw(-1); // Throw an exception if it could not be added
		}

		set_name(_name);
		set_path(_path);
	}
	/**
	* Free the texture data and remove it from the resource list.
	*/
	Texture::~Texture() {
		this->free();
		Texture::list.erase(id);
	}

	/**
	* @returns the number of Texture resources
	*/
	size_t Texture::get_amount() {
		return Texture::list.size();
	}
	/**
	* @param id the resource to get
	*
	* @returns the resource with the given id
	*/
	Texture* Texture::get(int id) {
		if (Texture::list.find(id) != Texture::list.end()) {
			return Texture::list.at(id);
		}
		return nullptr;
	}
	/**
	* @param name the name of the desired Texture
	*
	* @returns the Texture resource with the given name
	*/
	Texture* Texture::get_by_name(const std::string& name) {
		for (auto& tex : list) { // Iterate over the Textures in order to find the first one with the given name
			Texture* t = tex.second;
			if (t != nullptr) {
				if (t->get_name() == name) {
					return t; // Return the desired Texture on success
				}
			}
		}
		return nullptr; // Return nullptr on failure
	}
	/**
	* Initiliaze, load, and return a newly created Texture resource.
	* @param name the name to initialize the Texture with
	* @param path the path to initialize the Texture with
	*
	* @returns the newly loaded Texture
	*/
	Texture* Texture::add(const std::string& name, const std::string& path) {
		Texture* new_texture = new Texture(name, path);
		new_texture->load();
		return new_texture;
	}

	/**
	* Add the Texture to the appropriate resource list.
	*
	* @returns the Texture id
	*/
	int Texture::add_to_resources() {
		if (id < 0) { // If the resource needs to be added to the resource list
			id = Texture::next_id++;
			Texture::list.emplace(id, this); // Add the resource with it's new id
		}

		return id;
	}
	/**
	* Reset all resource variables for reinitialization.
	*
	* @retval 0 success
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
		origin = {0, 0};
		rotate = {0.5, 0.5};

		// Reset texture data
		texture = nullptr;
		is_loaded = false;
		has_draw_failed = false;

		return 0;
	}

	/**
	* @returns a map of all the information required to restore the Texture
	*/
	std::map<Variant,Variant> Texture::serialize() const {
		std::map<Variant,Variant> info;

		info["id"] = id;
		info["name"] = name;
		info["path"] = path;

		info["width"] = static_cast<int>(width);
		info["height"] = static_cast<int>(height);
		info["subimage_amount"] = static_cast<int>(subimage_amount);
		info["subimage_width"] = static_cast<int>(subimage_width);

		info["crop"] = {Variant(crop.x), Variant(crop.y), Variant(crop.w), Variant(crop.h)};
		info["speed"] = speed;
		info["origin_x"] = origin.first;
		info["origin_y"] = origin.second;
		info["rotate_x"] = rotate.first;
		info["rotate_y"] = rotate.second;

		info["texture"] = texture;
		info["is_loaded"] = is_loaded;
		info["has_draw_failed"] = has_draw_failed;

		return info;
	}
	/**
	* Restore the Texture from serialized data.
	* @param m the map of data to use
	*
	* @retval 0 success
	* @retval 1 failed to load the image
	*/
	int Texture::deserialize(std::map<Variant,Variant>& m) {
		id = m["id"].i;
		name = m["name"].s;
		path = m["name"].s;

		width = m["width"].i;
		height = m["height"].i;
		subimage_amount = m["subimage_amount"].i;
		subimage_width = m["subimage_width"].i;

		crop = {
			m["crop"].v[0].i,
			m["crop"].v[1].i,
			m["crop"].v[2].i,
			m["crop"].v[3].i,
		};
		speed = m["speed"].d;
		origin.first = m["origin_x"].i;
		origin.second = m["origin_y"].i;
		rotate.first = m["rotate_x"].d;
		rotate.second = m["rotate_y"].d;

		texture = nullptr;
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
	void Texture::print() const {
		Variant m (serialize());
		messenger::send({"engine", "texture"}, E_MESSAGE::INFO, "Texture " + m.to_str(true));
	}

	int Texture::get_id() const {
		return id;
	}
	std::string Texture::get_name() const {
		return name;
	}
	std::string Texture::get_path() const {
		return path;
	}
	std::pair<int,int> Texture::get_size() const {
		return std::make_pair(width, height);
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
	std::pair<int,int> Texture::get_origin() const {
		return origin;
	}
	std::pair<double,double> Texture::get_rotate() const {
		return rotate;
	}
	SDL_Texture* Texture::get_texture() const {
		return texture;
	}
	bool Texture::get_is_loaded() const {
		return is_loaded;
	}

	void Texture::set_name(const std::string& _name) {
		name = _name;
	}
	/**
	* Set the relative or absolute resource path.
	* @param _path the new path to use
	* @note If the first character is '/' then the path will be relative to
	*       the executable directory, otherwise it will be relative to the
	*       Textures resource directory.
	*/
	void Texture::set_path(const std::string& _path) {
		if (_path.front() == '/') {
			path = _path.substr(1);
		} else { // Append the path to the Texture directory if not root
			path = "resources/textures/"+_path;
		}
	}
	void Texture::set_speed(double _speed) {
		speed = _speed;
	}
	/**
	* Set the coordinates of the drawing origin.
	* @note Provide a negative value for either argument in order to leave it unchanged.
	* @param origin_x the new x-coordinate to draw from
	* @param origin_y the new y-coordinate to draw from
	*/
	void Texture::set_origin(int origin_x, int origin_y) {
		if (origin_x >= 0) {
			origin.first = origin_x;
		}
		if (origin_y >= 0) {
			origin.second = origin_y;
		}
	}
	void Texture::set_origin_center() {
		set_origin(subimage_width/2, height/2);
	}
	/**
	* Set the coordinates of the rotation origin.
	* @note All x- and y-coordinates of the rotation origin are given as a percentage from 0.0 to 1.0 of the image dimensions.
	* @note Provide a negative value for either argument in order to leave it unchanged.
	* @param rotate_x the new x-coordinate to rotate the texture around
	* @param rotate_y the new y-coordinate to rotate the Texture around
	*/
	void Texture::set_rotate(double rotate_x, double rotate_y) {
		if (rotate_x >= 0.0) {
			rotate.first = rotate_x;
		}
		if (rotate_y >= 0.0) {
			rotate.second = rotate_y;
		}
	}
	/**
	* Set subimage coordinates and generate OpenGL buffers if necessary.
	* @param _subimage_amount the amount of subimages to use
	* @param _subimage_width the width of each subimage
	*/
	void Texture::set_subimage_amount(int _subimage_amount, int _subimage_width) {
		// Reset the subimage properties
		subimage_amount = _subimage_amount;
		subimage_width = _subimage_width;

		if (get_option("is_headless").i) {
			return;
		}

		// Destroy all old texcoords
		if (!vbo_texcoords.empty()) {
			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();
		}

		// Convert the subimage width to a percentage of the full Texture width
		GLfloat w = GLfloat(subimage_width);
		if (width > 0) {
			w /= width;
		}
		// Generate the texcoords for each individual subimage
		for (size_t i=0; i<subimage_amount; i++) {
			GLuint _texcoord;
			GLfloat texcoords[] = {
				w*i,     0.0,
				w*(i+1), 0.0,
				w*(i+1), 1.0,
				w*i,     1.0,
			};
			glGenBuffers(1, &_texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, _texcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

			vbo_texcoords.push_back(_texcoord); // Add the texcoord to the list of subimages
		}
	}
	/**
	* Set a rectangle specifying the cropped size and generate OpenGL buffers for it if necessary.
	* @note Cropped images only support a single subimage.
	* @param _crop the rectangle to crop the image to
	*/
	void Texture::crop_image(SDL_Rect _crop) {
		if (get_option("is_headless").i) {
			crop = _crop;
			return;
		}

		if ((_crop.w == -1)&&(_crop.h == -1)) { // Reset the subimage properties if the image is being uncropped
			set_subimage_amount(1, width);
			crop = {0, 0, static_cast<int>(width), static_cast<int>(height)};
			return;
		} else if (_crop.w == -1) {
			crop.h = _crop.h;
		} else if (_crop.h == -1) {
			crop.w = _crop.w;
		} else {
			crop = _crop;
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

		// Convert the width and height of the crop rectangle to a percentage of the full Texture dimensions
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
	}

	/**
	* Load a OpenGL texture from the given surface.
	* @param tmp_surface the temporary surface to load from
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already been loaded
	*/
	int Texture::load_from_surface(SDL_Surface* tmp_surface) {
		if (is_loaded) { // If the Texture has already been loaded, output a warning
			messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to load OpenGL texture \"" + name + "\" from surface because it has already been loaded");
			return 1;
		}

		// Set the Texture dimensions
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
		glEnableVertexAttribArray(render::get_program()->get_location("v_position"));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(
			render::get_program()->get_location("v_position"),
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

		return 0;
	}
	/**
	* Load the SDL surface from the Texture's path.
	* @note The caller is responsible for freeing the surface.
	*
	* @returns the surface on success or nullptr on failure
	*/
	SDL_Surface* Texture::load_surface() const {
		SDL_Surface* surface = IMG_Load(path.c_str());
		if (surface == nullptr) { // If the surface could not be loaded, output a warning
			messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to load Texture surface \"" + name + "\": " + util::get_sdl_error());
			return nullptr;
		}
		return surface;
	}
	/**
	* Load the Texture from its path.
	*
	* @retval 0 success
	* @retval 1 failed to load since it's already loaded
	* @retval 2 failed to load since the engine is in headless mode
	* @retval 3 failed to load temporary surface
	*/
	int Texture::load() {
		if (is_loaded) { // Do not attempt to load the Texture if it has already been loaded
		       messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to load Texture \"" + name + "\" because it has already been loaded");
		       return 1;
		}

		if (get_option("is_headless").i) {
			return 2;
		}

		SDL_Surface* tmp_surface = load_surface(); // Load the Texture into a temporary surface
		if (tmp_surface == nullptr) {
			return 3;
		}

		load_from_surface(tmp_surface); // Load the surface into a texture
		SDL_FreeSurface(tmp_surface);

		return 0;
	}
	/**
	* Setup the Texture for use as a render target.
	* @param w the target width to use
	* @param h the target height to use
	*
	* @retval 0 success
	* @retval 1 failed to set as target since it's already loaded
	* @retval 2 failed to set as target since the engine is in headless mode
	* @retval 3 failed to initialize framebuffer
	*/
	int Texture::load_as_target(int w, int h) {
		if (is_loaded) { // Do not attempt to load the Texture if it has already been loaded
			messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to load Texture \"" + name + "\" because it has already been loaded");
			return 1;
		}

		if (get_option("is_headless").i) {
			return 2;
		}

		// Set the Texture dimensions and remove all cropping
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
		glEnableVertexAttribArray(render::get_program()->get_location("v_position"));
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glVertexAttribPointer(
			render::get_program()->get_location("v_position"),
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
			return 3;
		}

		glBindVertexArray(0); // Unbind VAO when done loading

		// Set the loaded booleans
		is_loaded = true;
		has_draw_failed = false;

		return 0;
	}
	/**
	* Free the Texture and delete all of its buffers.
	*
	* @retval 0 success
	*/
	int Texture::free() {
		if (!is_loaded) { // Do not attempt to free the data if the Texture has not been loaded
			return 0;
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

		return 0;
	}

	/**
	* Enable all required drawing buffers.
	*
	* @retval 0 success
	* @retval 1 failed to enable buffers since it's not loaded
	*/
	int Texture::drawing_begin() {
		if (!is_loaded) {
			return 1;
		}

		glBindVertexArray(vao); // Bind the VAO for the texture

		// Bind the texture
		glUniform1i(render::get_program()->get_location("f_texture"), 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

		return 0;
	}
	/**
	* Disable all required drawing buffers.
	*
	* @retval 0 success
	* @retval 1 failed to enable buffers since it's not loaded
	*/
	int Texture::drawing_end() {
		if (!is_loaded) {
			return 1;
		}

		glUniformMatrix4fv(render::get_program()->get_location("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the partial transformation matrix
		glUniformMatrix4fv(render::get_program()->get_location("rotation"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Reset the rotation matrix
		glUniform1i(render::get_program()->get_location("flip"), 0); // Reset the flip type

		glBindVertexArray(0); // Unbind the VAO

		return 0;
	}

	/**
	* Draw a given subimage of the Texture with the given attributes.
	* @param x the x-coordinate to draw the subimage at
	* @param y the y-coordinate to draw the subimage at
	* @param subimage the subimage of the Texture to draw
	* @param w the width to scale the subimage to
	* @param h the height to scale the subimage to
	* @param angle the number of degrees to rotate the subimage clockwise
	* @param color the color to paint the subimage in
	*
	* @retval 0 success
	* @retval 1 failed to draw since it's not loaded
	*/
	int Texture::draw_subimage(int x, int y, unsigned int subimage, int w, int h, double angle, RGBA color) {
		if (!is_loaded) { // Do not attempt to draw the subimage if it has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to draw Texture \"" + name + "\" because it is not loaded");
				has_draw_failed = true;
			}
			return 1;
		}

		SDL_Rect drect = {x, y, 0, 0}; // Create a rectangle to define the position and dimensions of the destination render

		// Determine the desired width and height of the render
		if ((w >= 0)&&(h >= 0)) { // If the width and height are provided to the function, use them as is
			drect.w = w;
			drect.h = h;
		} else { // Otherwise set the width and height to the same as the Texture, i.e. don't scale the render
			drect.w = width;
			drect.h = height;
			if (subimage_amount > 1) {
				drect.w = subimage_width;
			}
		}

		// Get the full width of the Texture to be used for scaling
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
			td.rotation = glm::translate(glm::mat4(1.0f), glm::vec3(rotate.first*rect_width, rotate.second*height, 0.0f));
			td.rotation = glm::rotate(td.rotation, static_cast<float>(util::degtorad(angle)), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate the subimage on the z-axis around the Texture's rotation origin at (rotate_x, rotate_y)
			td.rotation = glm::translate(td.rotation, glm::vec3(-rotate.first*rect_width, -rotate.second*height, 0.0f));
		}

		// Colorize the Texture with the given color
		td.color = glm::vec4(color.r, color.g, color.b, color.a); // Normalize the color values from 0.0 to 1.0
		td.color /= 255.0f;

		// Bind the texture coordinates of the current subimage
		td.buffer = vbo_texcoords[subimage];

		render::queue_texture(this, td);

		// If the Texture has reached the end of its subimage cycle, set the animation boolean
		if ((is_animated)&&(subimage == subimage_amount-1)) {
			is_animated = false;
		}

		return 0;
	}
	/**
	* Draw the Texture with a given subimage timing using the given attributes.
	* @param x the x-coordinate to draw the Texture at
	* @param y the y-coordinate to draw the Texture at
	* @param subimage_time the frame of animation to choose the subimage from
	* @param w the width to scale the Texture to
	* @param h the height to scale the Texture to
	* @param angle the number of degrees to rotate the Texture clockwise
	* @param color the color to paint the Texture in
	*
	* @returns whether the draw call failed or not
	* @see draw_subimage() for details
	*/
	int Texture::draw(int x, int y, Uint32 subimage_time, int w, int h, double angle, RGBA color) {
		// Calculate the current subimage to draw from the given animation frame
		unsigned int current_subimage = static_cast<unsigned int>(round(speed*(get_ticks()-subimage_time)/get_fps_goal())) % subimage_amount;
		if (current_subimage == 0) { // If the first frame is being drawn, set the animation boolean
			is_animated = true;
		}

		return draw_subimage(x, y, current_subimage, w, h, angle, color);
	}
	/**
	* Draw the Texture with a given subimage timing using the given attributes.
	* @note When the function is called with no other attributes, let them be values that will not affect the render.
	* @param x the x-coordinate to draw the Texture at
	* @param y the y-coordinate to draw the Texture at
	* @param subimage_time the frame of animation to choose the subimage from
	*
	* @returns whether the draw call failed or not
	* @see draw_subimage() for details
	*/
	int Texture::draw(int x, int y, Uint32 subimage_time) {
		return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255});
	}
	/**
	* Draw the Texture with the given TextureTransform.
	* @note Transforms were originally implemented to replace Backgrounds.
	* @param tr the transform data to use
	*
	* @retval 0 success
	* @retval 1 failed to draw since it's not loaded
	* @retval <0 a tile call failed
	*/
	int Texture::draw_transform(const TextureTransform& tr) {
		if (!is_loaded) { // Do not attempt to draw the Texture if it has not been loaded
			if (!has_draw_failed) { // If the draw call hasn't failed before, output a warning
				messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to draw Texture \"" + name + "\" because it is not loaded");
				has_draw_failed = true;
			}
			return 1;
		}

		if (tr.is_stretched) { // If the Texture should be stretched, then draw it without animation
			draw(0, 0, 0, get_room_size().first, get_room_size().second, 0.0, {255, 255, 255, 255});
		} else {
			const int dt_fps = get_ticks()/get_fps_goal();
			int dx = tr.horizontal_speed*dt_fps;
			int dy = tr.vertical_speed*dt_fps;

			SDL_Rect rect = {tr.x+dx, tr.y+dy, static_cast<int>(width), static_cast<int>(height)};

			if ((tr.is_horizontal_tile)&&(tr.is_vertical_tile)) {
				int ret = 0;

				const int rh = get_room_size().second;
				while (rect.y-rect.h < rh) { // Tile as many horizontal lines as necessary to fill the window to the bottom
					ret += tile_horizontal(rect); // Tile the Texture across the row
					rect.y += rect.h; // Move to the below row
				}
				rect.y = tr.y + dy - rect.h; // Reset the row to above the first
				while (rect.y+rect.h > 0) { // Tile as many horizontal lines as necessary to fill the window to the top
					ret += tile_horizontal(rect); // Tile the Texture across the row
					rect.y -= rect.h; // Move to the above row
				}

				return -ret;
			} else if (tr.is_horizontal_tile) {
				return -tile_horizontal(rect);
			} else if (tr.is_vertical_tile) {
				return -tile_vertical(rect);
			} else {
				return draw(rect.x, rect.y, 0);
			}
		}

		return 0;
	}
	/**
	* Repeatedly draw the Texture horizontally across the screen.
	* @param r the rectangle to draw
	*
	* @retval 0 success
	* @retval nonzero a draw call failed
	*/
	int Texture::tile_horizontal(const SDL_Rect& r) {
		int ret = 0;

		SDL_Rect dest (r);
		const int rw = get_room_size().first;

		while (dest.x < rw) { // Continue drawing to the right until the rectangle is past the right side of the window
			ret += draw(dest.x, dest.y, 0);
			dest.x += dest.w; // Move the rectangle on right to the next tile
		}

		dest.x = r.x - dest.w; // Reset the rectangle start
		while (dest.x + dest.w > 0) { // Continue drawing to the left until the rectangle is past the left side of the window
			ret += draw(dest.x, dest.y, 0);
			dest.x -= dest.w; // Move the rectangle on left to the next tile
		}

		return ret;
	}
	/**
	* Repeatedly draw the Texture vertically down the screen.
	* @param r the rectangle to draw
	*
	* @retval 0 success
	* @retval nonzero a draw call failed
	*/
	int Texture::tile_vertical(const SDL_Rect& r) {
		int ret = 0;

		SDL_Rect dest (r);
		const int rh = get_room_size().second;

		while (dest.y < rh) { // Continue drawing to the bottom until the rectnagle is past the bottom of the window
			ret += draw(dest.x, dest.y, 0);
			dest.y += dest.h; // Move the rectangle on down to the next tile
		}

		dest.y = r.y - dest.h; // Reset the rectangle start
		while (dest.y + dest.h > 0) { // Continue drawing to the top until the rectangle is past the top of the window
			ret += draw(dest.x, dest.y, 0);
			dest.y -= dest.h; // Move the rectangle on up to the next tile
		}

		return ret;
	}
	/**
	* Set the Texture as the render target.
	*
	* @returns the OpenGL framebuffer index
	*/
	GLuint Texture::set_as_target() {
		if (framebuffer == static_cast<GLuint>(-1)) {
			messenger::send({"engine", "texture"}, E_MESSAGE::WARNING, "Failed to set render target: uninitialized framebuffer");
			return -1;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		// Bind the framebuffer to the empty Texture
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gl_texture, 0);
		GLenum buffer[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, buffer); // Enable drawing to the framebuffer

		return framebuffer;
	}
}

#endif // BEE_TEXTURE
