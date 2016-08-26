/*
* Copyright (c) 2015-16 Luke Montalvo <lukemontalvo@gmail.com>
*
* This file is part of BEE.
* BEE is free software and comes with ABSOLUTELY NO WARANTY.
* See LICENSE for more details.
*/

#ifndef _BEE_SPRITE
#define _BEE_SPRITE 1

#include "sprite.hpp" // Include the class resource header

/*
* BEE::Sprite::Sprite() - Construct the sprite, set its engine pointer, and reset all variables
* ! This constructor should only be used for temporary sprites (e.g. framebuffers), the other should be used for all other cases
*/
BEE::Sprite::Sprite () {
	if (BEE::resource_list->sprites.game != nullptr) {
		game = BEE::resource_list->sprites.game; // Set the engine pointer
	}

	reset(); // Reset all resource variables
}
/*
* BEE::Sprite::Sprite() - Construct the sprite, reset all variables, add it to the sprite resource list, and set the new name and path
* @new_name: the name of the sprite to use
* @new_path: the path of the sprite's image
*/
BEE::Sprite::Sprite (std::string new_name, std::string path) {
	reset(); // Reset all resource variables

	add_to_resources("resources/sprites/"+path); // Add the sprite to the appropriate resource list
	if (id < 0) { // If the sprite could not be added to the resource list
		game->messenger_send({"engine", "resource"}, BEE_MESSAGE_WARNING, "Failed to add sprite resource: " + path);
		throw(-1); // Throw an exception
	}

	set_name(new_name); // Set the sprite name
	set_path(path); // Set the sprite image path
}
/*
* BEE::Sprite::~Sprite() - Free the sprite data and remove it from the resource list
*/
BEE::Sprite::~Sprite() {
	free(); // Free all sprite data
	BEE::resource_list->sprites.remove_resource(id); // Remove the sprite from the resource list
}
/*
* BEE::Sprite::add_to_resources() - Add the sprite to
*/
int BEE::Sprite::add_to_resources(std::string path) {
	int list_id = -1;
	if (id >= 0) {
		if (path == image_path) {
			return 1; // Return 1 if the sprite already has already been added to the resource list
		}
		BEE::resource_list->sprites.remove_resource(id); // Remove the sprite from the resource list if the existing entry is not the same
		id = -1; // Reset the sprite id
	}

	id = BEE::resource_list->sprites.add_resource(this);
	BEE::resource_list->sprites.set_resource(id, this);

	if (BEE::resource_list->sprites.game != nullptr) {
		game = BEE::resource_list->sprites.game;
	}

	return 0;
}
int BEE::Sprite::reset() {
	free();

	name = "";
	image_path = "";
	width = 0;
	height = 0;
	subimage_amount = 1;
	subimage_width = 0;
	crop = {0, 0, 0, 0};
	speed = 0.0;
	alpha = 1.0;
	is_animated = false;
	origin_x = 0;
	origin_y = 0;
	rotate_x = 0.5;
	rotate_y = 0.5;

	texture = nullptr;
	is_loaded = false;
	subimages.clear();
	has_draw_failed = false;
	is_lightable = true;

	return 0;
}
int BEE::Sprite::print() {
	std::stringstream s;
	s <<
	"Sprite { "
	"\n	id		" << id <<
	"\n	name		" << name <<
	"\n	image_path	" << image_path <<
	"\n	width		" << width <<
	"\n	height		" << height <<
	"\n	subimage_amount	" << subimage_amount <<
	"\n	subimage_width	" << subimage_width <<
	"\n	speed		" << speed <<
	"\n	alpha		" << alpha <<
	"\n	origin_x	" << origin_x <<
	"\n	origin_y	" << origin_y <<
	"\n	texture		" << texture <<
	"\n	is_loaded	" << is_loaded <<
	"\n}\n";
	game->messenger_send({"engine", "resource"}, BEE_MESSAGE_INFO, s.str());

	return 0;
}
int BEE::Sprite::get_id() {
	return id;
}
std::string BEE::Sprite::get_name() {
	return name;
}
std::string BEE::Sprite::get_path() {
	return image_path;
}
int BEE::Sprite::get_width() {
	return width;
}
int BEE::Sprite::get_height() {
	return height;
}
int BEE::Sprite::get_subimage_amount() {
	return subimage_amount;
}
int BEE::Sprite::get_subimage_width() {
	return subimage_width;
}
double BEE::Sprite::get_speed() {
	return speed;
}
double BEE::Sprite::get_alpha() {
	return alpha;
}
bool BEE::Sprite::get_is_animated() {
	return is_animated;
}
int BEE::Sprite::get_origin_x() {
	return origin_x;
}
int BEE::Sprite::get_origin_y() {
	return origin_y;
}
double BEE::Sprite::get_rotate_x() {
	return rotate_x;
}
double BEE::Sprite::get_rotate_y() {
	return rotate_y;
}
SDL_Texture* BEE::Sprite::get_texture() {
	return texture;
}
bool BEE::Sprite::get_is_loaded() {
	return is_loaded;
}
bool BEE::Sprite::get_is_lightable() {
	return is_lightable;
}

int BEE::Sprite::set_name(std::string new_name) {
	name = new_name;
	return 0;
}
int BEE::Sprite::set_path(std::string path) {
	add_to_resources("resources/sprites/"+path);
	image_path = "resources/sprites/"+path;
	return 0;
}
int BEE::Sprite::set_subimage_amount(int new_subimage_amount, int new_subimage_width) {
	subimages.clear();
	subimage_amount = new_subimage_amount;
	subimage_width = new_subimage_width;

	for (int i=0; i<subimage_amount; i++) {
		subimages.push_back({i*subimage_width, 0, subimage_width, height});
	}

	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		if (!vbo_texcoords.empty()) {
			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();
		}

		GLfloat w = subimage_width;
		if (width > 0) {
			w /= width;
		}
		for (int i=0; i<subimage_amount; i++) {
			GLuint new_texcoord;
			GLfloat texcoords[] = {
				w*i, 0.0,
				w*(i+1), 0.0,
				w*(i+1), 1.0,
				w*i, 1.0,
			};
			glGenBuffers(1, &new_texcoord);
			glBindBuffer(GL_ARRAY_BUFFER, new_texcoord);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

			vbo_texcoords.push_back(new_texcoord);
		}
	}

	return 0;
}
int BEE::Sprite::crop_image(SDL_Rect new_crop) {
	crop = new_crop;

	if ((crop.w == -1)&&(crop.h == -1)) {
		set_subimage_amount(1, width);
		crop = {0, 0, width, height};
		return 0;
	}

	set_subimage_amount(1, crop.w);
	subimages[0] = crop;

	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		if (!vbo_texcoords.empty()) {
			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();
		}


		GLfloat x, y, w, h;
		x = crop.x; y = crop.y;
		w = crop.w; h = crop.h;
		x /= width; w /= width;
		y /= height; h /= height;

		GLuint new_texcoord;
		GLfloat texcoords[] = {
			x, y,
			x+w, y,
			x+w, y+h,
			x, y+h
		};
		glGenBuffers(1, &new_texcoord);
		glBindBuffer(GL_ARRAY_BUFFER, new_texcoord);
		glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);

		vbo_texcoords.push_back(new_texcoord);
	}

	return 0;
}
int BEE::Sprite::crop_image_width(int new_crop_width) {
	crop.w = new_crop_width;
	return crop_image(crop);
}
int BEE::Sprite::crop_image_height(int new_crop_height) {
	crop.h = new_crop_height;
	return crop_image(crop);
}
int BEE::Sprite::set_speed(double new_speed) {
	speed = new_speed;
	return 0;
}
int BEE::Sprite::set_alpha(double new_alpha) {
	alpha = new_alpha;
	return 0;
}
int BEE::Sprite::set_origin_x(int new_origin_x) {
	origin_x = new_origin_x;
	return 0;
}
int BEE::Sprite::set_origin_y(int new_origin_y) {
	origin_y = new_origin_y;
	return 0;
}
int BEE::Sprite::set_origin_xy(int new_origin_x, int new_origin_y) {
	set_origin_x(new_origin_x);
	set_origin_y(new_origin_y);
	return 0;
}
int BEE::Sprite::set_origin_center() {
	set_origin_x(subimage_width/2);
	set_origin_y(height/2);
	return 0;
}
int BEE::Sprite::set_rotate_x(double new_rotate_x) {
	rotate_x = new_rotate_x;
	return 0;
}
int BEE::Sprite::set_rotate_y(double new_rotate_y) {
	rotate_y = new_rotate_y;
	return 0;
}
int BEE::Sprite::set_rotate_xy(double new_rotate_x, double new_rotate_y) {
	set_rotate_x(new_rotate_x);
	set_rotate_y(new_rotate_y);
	return 0;
}
int BEE::Sprite::set_rotate_center() {
	set_rotate_x(0.5);
	set_rotate_y(0.5);
	return 0;
}
int BEE::Sprite::set_is_lightable(bool new_is_lightable) {
	is_lightable = new_is_lightable;
	return 0;
}

int BEE::Sprite::load_from_surface(SDL_Surface* tmp_surface) {
	if (!is_loaded) {
		width = tmp_surface->w;
		height = tmp_surface->h;
		if (subimage_amount <= 1) {
			set_subimage_amount(1, width);
		} else {
			set_subimage_amount(subimage_amount, width/subimage_amount);
		}
		crop = {0, 0, width, height};

		if (game->options->renderer_type != BEE_RENDERER_SDL) {
			GLfloat vertices[] = {
				0.0, 0.0,
				(GLfloat)subimage_width, 0.0,
				(GLfloat)subimage_width, (GLfloat)height,
				0.0, (GLfloat)height,
			};
			glGenBuffers(1, &vbo_vertices);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

			GLushort elements[] = {
				0, 1, 2,
				2, 3, 0,
			};
			glGenBuffers(1, &ibo);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

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

			is_loaded = true;
			has_draw_failed = false;
		} else {
			texture = SDL_CreateTextureFromSurface(game->renderer, tmp_surface);
			if (texture == nullptr) {
				game->messenger_send({"engine", "sprite"}, BEE_MESSAGE_WARNING, "Failed to create texture from surface: " + get_sdl_error());
				return 1;
			}

			SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(texture, alpha*255);

			is_loaded = true;
			has_draw_failed = false;
		}
	} else {
		game->messenger_send({"engine", "sprite"}, BEE_MESSAGE_WARNING, "Failed to load sprite from surface because it has already been loaded");
		return 1;
	}

	return 0;
}
int BEE::Sprite::load() {
	if (!is_loaded) {
		SDL_Surface* tmp_surface;
		tmp_surface = IMG_Load(image_path.c_str());
		if (tmp_surface == nullptr) {
			game->messenger_send({"engine", "sprite"}, BEE_MESSAGE_WARNING, "Failed to load sprite \"" + name + "\": " + IMG_GetError());
			return 1;
		}

		load_from_surface(tmp_surface);
		SDL_FreeSurface(tmp_surface);
	}
	return 0;
}
int BEE::Sprite::free() {
	if (is_loaded) {
		if (game->options->renderer_type != BEE_RENDERER_SDL) {
			glDeleteBuffers(1, &vbo_vertices);

			for (auto& t : vbo_texcoords) {
				glDeleteBuffers(1, &t);
			}
			vbo_texcoords.clear();

			glDeleteBuffers(1, &ibo);
			glDeleteTextures(1, &gl_texture);
			glDeleteFramebuffers(1, &framebuffer);
		} else {
			SDL_DestroyTexture(texture);
			texture = nullptr;
		}
		is_loaded = false;
	}
	return 0;
}
int BEE::Sprite::draw_subimage(int x, int y, int current_subimage, int w, int h, double angle, RGBA new_color, SDL_RendererFlip flip, bool is_hud) {
	if (!is_loaded) {
		if (!has_draw_failed) {
			game->messenger_send({"engine", "sprite"}, BEE_MESSAGE_WARNING, "Failed to draw sprite \"" + name + "\" because it is not loaded");
			has_draw_failed = true;
		}
		return 1;
	}

	SDL_Rect drect = {x, y, 0, 0};

	if ((w >= 0)&&(h >= 0)) {
		drect.w = w;
		drect.h = h;
	} else if (subimage_amount > 1) {
		drect.w = subimage_width;
		drect.h = height;
	} else {
		drect.w = width;
		drect.h = height;
	}

	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		int rect_width = width;
		if (subimage_amount > 1) {
			rect_width = subimage_width;
		}

		if (w <= 0) {
			w = rect_width;
		}
		if (h <= 0) {
			h = height;
		}

		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3((float)drect.x, (float)drect.y, 0.0f));
		model = glm::scale(model, glm::vec3((float)w/rect_width, (float)h/height, 1.0f));
		if (angle != 0.0) {
			glm::mat4 rotation = glm::translate(glm::mat4(1.0f), glm::vec3((float)rect_width*rotate_x, (float)height*rotate_y, 0.0f));
			rotation = glm::rotate(rotation, (float)degtorad(angle), glm::vec3(0.0f, 0.0f, 1.0f));
			rotation = glm::translate(rotation, glm::vec3(-(float)rect_width*rotate_x, -(float)height*rotate_y, 0.0f));
			glUniformMatrix4fv(game->rotation_location, 1, GL_FALSE, glm::value_ptr(rotation));
		}
		glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(model));

		glUniform1i(game->texture_location, 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

		glm::vec4 color = glm::vec4((float)new_color.r/255.0f, (float)new_color.g/255.0f, (float)new_color.b/255.0f, (float)new_color.a/255.0f);
		glUniform4fv(game->colorize_location, 1, glm::value_ptr(color));

		int f = 0;
		if (flip & SDL_FLIP_HORIZONTAL) {
			f += 1;
		}
		if (flip & SDL_FLIP_VERTICAL) {
			f += 2;
		}
		glUniform1i(game->flip_location, f);

		// Shadows
		if (is_lightable) {
			LightableData* l = new LightableData();
			l->position = glm::vec4((float)drect.x, (float)drect.y, 0.0f, 0.0f);
			l->mask.push_back(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
			l->mask.push_back(glm::vec4((float)rect_width, 0.0f, 0.0f, 0.0f));
			l->mask.push_back(glm::vec4((float)rect_width, (float)height, 0.0f, 0.0f));
			l->mask.push_back(glm::vec4(0.0f, (float)height, 0.0f, 0.0f));
			game->get_current_room()->add_lightable(l);
		}

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

		glEnableVertexAttribArray(game->fragment_location);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords[current_subimage]);
		glVertexAttribPointer(
			game->fragment_location,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			0
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

		glDisableVertexAttribArray(game->vertex_location);
		glDisableVertexAttribArray(game->fragment_location);

		glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(game->flip_location, 0);
		glUniformMatrix4fv(game->rotation_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	} else {
		if ((game->get_current_room()->get_is_views_enabled())&&(!is_hud)) {
			if (game->get_current_room()->get_current_view() != nullptr) {
				drect.x += game->get_current_room()->get_current_view()->view_x;
				drect.y += game->get_current_room()->get_current_view()->view_y;
			}
		}

		if (game->is_on_screen(drect)) {
			SDL_SetTextureColorMod(texture, new_color.r, new_color.g, new_color.b);
			SDL_SetTextureAlphaMod(texture, new_color.a);
			SDL_Point r = {(int)(rotate_x*subimage_width), (int)(rotate_y*height)};

			SDL_SetTextureBlendMode(texture, game->draw_get_blend());

			if (!subimages.empty()) {
				SDL_RenderCopyEx(game->renderer, texture, &subimages[current_subimage], &drect, angle, &r, flip);
			} else {
				SDL_RenderCopyEx(game->renderer, texture, &crop, &drect, angle, &r, flip);
			}
		}
	}

	if ((is_animated)&&(current_subimage == subimage_amount-1)) {
		is_animated = false;
	}

	return 0;
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, int w, int h, double angle, RGBA new_color, SDL_RendererFlip flip, bool is_hud) {
	int current_subimage = (int)round(speed*(game->get_ticks()-subimage_time)/game->fps_goal) % subimage_amount;
	if (current_subimage == 0) {
		is_animated = true;
	}

	return draw_subimage(x, y, current_subimage, w, h, angle, new_color, flip, is_hud);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time) {
	return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}, SDL_FLIP_NONE, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, int w, int h) {
	return draw(x, y, subimage_time, w, h, 0.0, {255, 255, 255, 255}, SDL_FLIP_NONE, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, double angle) {
	return draw(x, y, subimage_time, -1, -1, angle, {255, 255, 255, 255}, SDL_FLIP_NONE, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, RGBA color) {
	return draw(x, y, subimage_time, -1, -1, 0.0, color, SDL_FLIP_NONE, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, SDL_RendererFlip flip) {
	return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}, flip, false);
}
int BEE::Sprite::draw(int x, int y, Uint32 subimage_time, bool is_hud) {
	return draw(x, y, subimage_time, -1, -1, 0.0, {255, 255, 255, 255}, SDL_FLIP_NONE, is_hud);
}
int BEE::Sprite::draw_simple(SDL_Rect* source, SDL_Rect* dest) {
	if (!is_loaded) {
		return 1;
	}
	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		return 2;
	}
	return SDL_RenderCopy(game->renderer, texture, source, dest);
}
int BEE::Sprite::draw_array(const std::list<SpriteDrawData*>& draw_list, const std::vector<glm::mat4>& rotation_cache, RGBA new_color, SDL_RendererFlip flip, bool is_hud) {
	if (!is_loaded) {
		if (!has_draw_failed) {
			game->messenger_send({"engine", "sprite"}, BEE_MESSAGE_WARNING, "Failed to draw sprite \"" + name + "\" because it is not loaded");
			has_draw_failed = true;
		}
		return 1;
	}

	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		glUniform1i(game->texture_location, 0);
		glBindTexture(GL_TEXTURE_2D, gl_texture);

		glm::vec4 color = glm::vec4((float)new_color.r/255.0f, (float)new_color.g/255.0f, (float)new_color.b/255.0f, (float)new_color.a/255.0f);
		glUniform4fv(game->colorize_location, 1, glm::value_ptr(color));

		int f = 0;
		if (flip & SDL_FLIP_HORIZONTAL) {
			f += 1;
		}
		if (flip & SDL_FLIP_VERTICAL) {
			f += 2;
		}
		glUniform1i(game->flip_location, f);

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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		int size;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);

		Uint32 t = game->get_ticks();
		for (auto& s : draw_list) {
			int current_subimage = (int)round(speed*(t-s->subimage_time)/game->get_fps_goal()) % subimage_amount;
			if (current_subimage == 0) {
				is_animated = true;
			}

			SDL_Rect drect = {s->x, s->y, 0, 0};

			if ((s->w >= 0)&&(s->h >= 0)) {
				drect.w = s->w;
				drect.h = s->h;
			} else if (subimage_amount > 1) {
				drect.w = subimage_width;
				drect.h = height;
			} else {
				drect.w = width;
				drect.h = height;
			}

			int rect_width = width;
			if (subimage_amount > 1) {
				rect_width = subimage_width;
			}

			if (s->w <= 0) {
				s->w = rect_width;
			}
			if (s->h <= 0) {
				s->h = height;
			}

			glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3((float)drect.x, (float)drect.y, 0.0f));
			model = glm::scale(model, glm::vec3((float)s->w/rect_width, (float)s->h/height, 1.0));
			if (s->angle != 0.0) {
				glUniformMatrix4fv(game->rotation_location, 1, GL_FALSE, glm::value_ptr(rotation_cache[s->angle]));
			}
			glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(model));

			glEnableVertexAttribArray(game->fragment_location);
			glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoords[current_subimage]);
			glVertexAttribPointer(
				game->fragment_location,
				2,
				GL_FLOAT,
				GL_FALSE,
				0,
				0
			);

			glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
		}

		glDisableVertexAttribArray(game->vertex_location);
		glDisableVertexAttribArray(game->fragment_location);

		glUniformMatrix4fv(game->model_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(game->flip_location, 0);
		glUniformMatrix4fv(game->rotation_location, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	} else {
		SDL_SetTextureColorMod(texture, new_color.r, new_color.g, new_color.b);
		if (new_color.a == 0) {
			SDL_SetTextureAlphaMod(texture, alpha*255);
		} else {
			SDL_SetTextureAlphaMod(texture, new_color.a);
		}

		Uint32 t = game->get_ticks();
		for (auto& s : draw_list) {
			int current_subimage = (int)round(speed*(t-s->subimage_time)/game->get_fps_goal()) % subimage_amount;
			if (current_subimage == 0) {
				is_animated = true;
			}

			SDL_Rect drect = {s->x, s->y, 0, 0};
			if ((game->get_current_room()->get_is_views_enabled())&&(!is_hud)) {
				if (game->get_current_room()->get_current_view() != nullptr) {
					drect.x += game->get_current_room()->get_current_view()->view_x;
					drect.y += game->get_current_room()->get_current_view()->view_y;
				}
			}

			if ((s->w >= 0)&&(s->h >= 0)) {
				drect.w = s->w;
				drect.h = s->h;
			} else if (subimage_amount > 1) {
				drect.w = subimage_width;
				drect.h = height;
			} else {
				drect.w = width;
				drect.h = height;
			}

			if (!subimages.empty()) {
				SDL_RenderCopyEx(game->renderer, texture, &subimages[current_subimage], &drect, s->angle, nullptr, flip);
			} else {
				SDL_RenderCopyEx(game->renderer, texture, nullptr, &drect, s->angle, nullptr, flip);
			}
		}
	}

	return 0;
}
int BEE::Sprite::set_as_target(int w, int h) {
	if (is_loaded) {
		free();
	}

	width = w;
	height = h;
	set_subimage_amount(1, width);
	crop = {0, 0, width, height};

	if (game->options->renderer_type != BEE_RENDERER_SDL) {
		GLfloat vertices[] = {
			0.0, 0.0,
			(GLfloat)width, 0.0,
			(GLfloat)width, (GLfloat)height,
			0.0, (GLfloat)height,
		};
		glGenBuffers(1, &vbo_vertices);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_vertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		GLushort elements[] = {
			0, 1, 2,
			2, 3, 0,
		};
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

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
			0
		);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gl_texture, 0);
		GLenum buffer[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, buffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			free();
			return 0;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		is_loaded = true;
		has_draw_failed = false;

		return (int)framebuffer;
	} else {
		texture = SDL_CreateTexture(game->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
		if (texture == nullptr) {
			game->messenger_send({"engine", "sprite"}, BEE_MESSAGE_WARNING, "Failed to create a blank texture: " + get_sdl_error());
			return 1;
		}

		SDL_SetRenderTarget(game->renderer, texture);

		is_loaded = true;
		has_draw_failed = false;
	}

	return 0;
}
int BEE::Sprite::set_as_target() {
	return set_as_target(game->get_width(), game->get_height());
}

#endif // _BEE_SPRITE
